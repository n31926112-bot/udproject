"""
collector.py - Orchestrator entry point for PortSync Python data collection.
Demonstrates: Stream orchestration, CLI parsing (argparse), protocol validation, lazy loading.
"""
import sys
import os

# Ensure python/ module directory is on sys.path regardless of execution CWD
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import argparse
import datetime
import csv
import time
from typing import List

from protocol import (
    encode_hello, encode_trade, encode_price, encode_hist,
    encode_news, encode_batch_end, encode_bye
)
from net_client import NetClient
from brokers.groww_sample import GrowwSampleAdapter
from brokers.zerodha_sample import ZerodhaSampleAdapter
from market import MarketDataEngine
from news import NewsEngine

def main():
    parser = argparse.ArgumentParser(description="PortSync Python Market Data Collector")
    parser.add_argument("--mode", choices=["sample", "live"], default="sample", help="Mode: sample or live")
    parser.add_argument("--interval", type=float, default=5.0, help="Polling interval in seconds")
    parser.add_argument("--port", type=int, default=5050, help="C++ TCP server port")
    parser.add_argument("--rounds", type=int, default=0, help="Number of price update rounds (0 = infinite)")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for sample mode")

    args = parser.parse_args()

    print(f"=== PortSync Collector Started (Mode: {args.mode.upper()}, Port: {args.port}) ===")

    # 1. Connect to C++ TCP Server
    client = NetClient(port=args.port)
    if not client.connect():
        print("[ERROR] Could not connect to C++ server. Exiting.")
        sys.exit(1)

    # 2. Handshake
    client.send_line(encode_hello("PortSync", 1))

    # 3. Load & Send Trades
    groww = GrowwSampleAdapter()
    zerodha = ZerodhaSampleAdapter()
    trades = groww.get_trades() + zerodha.get_trades()

    # Sort trades chronologically before sending
    trades.sort(key=lambda t: t.date_str)
    print(f"[COLLECTOR] Sending {len(trades)} trades across brokers...")
    for t in trades:
        client.send_line(encode_trade(t.broker, t.ticker, t.side, t.qty, t.price, t.date_str))

    # 4. Load & Send Historical Closes (from history.csv)
    base_dir = os.path.dirname(os.path.abspath(__file__))
    hist_csv = os.path.join(base_dir, "sample_data", "history.csv")
    if os.path.exists(hist_csv):
        print("[COLLECTOR] Sending historical daily closing prices...")
        with open(hist_csv, "r", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                client.send_line(encode_hist(row["ticker"], row["date"], float(row["close"])))

    # 5. Load & Send News
    news_engine = NewsEngine()
    news_items = news_engine.get_news(args.mode)
    print(f"[COLLECTOR] Sending {len(news_items)} news items...")
    for ticker, date_str, headline in news_items:
        client.send_line(encode_news(ticker, date_str, headline))

    # 6. Send Initial Prices
    market_engine = MarketDataEngine(seed=args.seed)
    tickers = list(set(t.ticker for t in trades))
    initial_prices = market_engine.get_prices(mode=args.mode, tickers=tickers)

    now_time = datetime.datetime.now().strftime("%H:%M:%S")
    for t, p in initial_prices.items():
        client.send_line(encode_price(t, p, now_time))

    # Initial Batch Complete
    batch_seq = 1
    client.send_line(encode_batch_end(batch_seq))
    print(f"[COLLECTOR] Initial sync batch {batch_seq} completed.")

    # 7. Periodic Price Update Loop
    round_count = 0
    try:
        while True:
            if args.rounds > 0 and round_count >= args.rounds:
                break
            time.sleep(args.interval)
            round_count += 1
            batch_seq += 1

            prices = market_engine.get_prices(mode=args.mode, tickers=tickers)
            curr_time = datetime.datetime.now().strftime("%H:%M:%S")
            print(f"[COLLECTOR] Round {round_count}: Streaming near-real-time price updates...")

            for t, p in prices.items():
                client.send_line(encode_price(t, p, curr_time))

            client.send_line(encode_batch_end(batch_seq))

    except KeyboardInterrupt:
        print("\n[COLLECTOR] Interrupted by user.")

    # 8. Termination
    client.send_line(encode_bye())
    client.close()
    print("[COLLECTOR] Data streaming session finished cleanly.")

if __name__ == "__main__":
    main()
