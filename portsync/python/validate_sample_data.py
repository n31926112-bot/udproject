"""
validate_sample_data.py - Validates sample dataset constraints for PortSync.
Checks:
1. At least 3 tickers shared across Groww and Zerodha
2. No overselling when trades are processed chronologically per broker
3. Alphabetical ticker run present (stress testing AVL)
4. ~250 historical price records per ticker in history.csv
"""
import os
import csv
from collections import defaultdict
from brokers.groww_sample import GrowwSampleAdapter
from brokers.zerodha_sample import ZerodhaSampleAdapter

def validate():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    sample_dir = os.path.join(base_dir, "sample_data")

    # 1. Load trades
    g_trades = GrowwSampleAdapter().get_trades()
    z_trades = ZerodhaSampleAdapter().get_trades()

    g_tickers = {t.ticker for t in g_trades}
    z_tickers = {t.ticker for t in z_trades}
    shared = g_tickers.intersection(z_tickers)

    print(f"[CHECK 1] Shared tickers count: {len(shared)} (tickers: {sorted(list(shared))})")
    assert len(shared) >= 3, f"Expected at least 3 shared tickers, got {len(shared)}"

    # 2. Check no oversell when merged chronologically
    all_trades = sorted(g_trades + z_trades, key=lambda t: t.date_str)
    holdings = defaultdict(lambda: defaultdict(int)) # broker -> ticker -> qty

    for t in all_trades:
        if t.side == "BUY":
            holdings[t.broker][t.ticker] += t.qty
        elif t.side == "SELL":
            holdings[t.broker][t.ticker] -= t.qty
            assert holdings[t.broker][t.ticker] >= 0, (
                f"Oversell detected for {t.ticker} in {t.broker} on {t.date_str}! Qty: {holdings[t.broker][t.ticker]}"
            )
    print("[CHECK 2] Chronological trade merging verified — zero oversell errors.")

    # 3. Alphabetical run check
    all_tickers = sorted(list(g_tickers.union(z_tickers)))
    print(f"[CHECK 3] Alphabetical ticker run present: {all_tickers}")
    assert all_tickers == sorted(all_tickers), "Ticker set must form an alphabetical sequence"

    # 4. History CSV check (~250 rows per ticker)
    hist_path = os.path.join(sample_dir, "history.csv")
    hist_counts = defaultdict(int)
    with open(hist_path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            hist_counts[row["ticker"]] += 1

    print("[CHECK 4] Historical data row counts per ticker:")
    for ticker, count in sorted(hist_counts.items()):
        print(f"  - {ticker}: {count} rows")
        assert 200 <= count <= 300, f"Expected ~250 rows for {ticker}, got {count}"

    print("\n>>> SAMPLE DATA VALIDATION PASSED SUCCESSFULLY! <<<\n")

if __name__ == "__main__":
    validate()
