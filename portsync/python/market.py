"""
market.py - Market price generator supporting sample random-walk and optional live yfinance polling.
"""
import os
import json
import random
import time
from typing import Dict, List, Tuple

class MarketDataEngine:
    def __init__(self, sample_json_path: str = None, seed: int = None):
        if sample_json_path is None:
            base_dir = os.path.dirname(os.path.abspath(__file__))
            sample_json_path = os.path.join(base_dir, "sample_data", "prices.json")
        self.sample_json_path = sample_json_path
        self.current_prices: Dict[str, float] = {}

        if seed is not None:
            random.seed(seed)

        self._load_base_prices()

    def _load_base_prices(self):
        if os.path.exists(self.sample_json_path):
            with open(self.sample_json_path, "r", encoding="utf-8") as f:
                self.current_prices = json.load(f)

    def get_prices(self, mode: str = "sample", tickers: List[str] = None) -> Dict[str, float]:
        if tickers is None:
            tickers = list(self.current_prices.keys())

        if mode == "live":
            prices = self._fetch_live_yfinance(tickers)
            if prices:
                return prices
            print("[MARKET WARN] Fallback to sample random walk for live market fetch failure.")

        # Sample random walk
        updated = {}
        for t in tickers:
            base = self.current_prices.get(t, 1000.0)
            # small random percentage change (-0.5% to +0.5%)
            change = random.uniform(-0.005, 0.005)
            new_price = round(base * (1 + change), 2)
            self.current_prices[t] = new_price
            updated[t] = new_price
        return updated

    def _fetch_live_yfinance(self, tickers: List[str]) -> Dict[str, float]:
        """Lazy import of yfinance for live price fetching with .NS extension."""
        try:
            import yfinance as yf
            yf_symbols = [f"{t}.NS" for t in tickers]
            data = yf.download(yf_symbols, period="1d", interval="1m", progress=False)
            results = {}
            if "Close" in data and not data["Close"].empty:
                latest_row = data["Close"].iloc[-1]
                for t in tickers:
                    yf_sym = f"{t}.NS"
                    if yf_sym in latest_row and not math.isnan(latest_row[yf_sym]):
                        results[t] = round(float(latest_row[yf_sym]), 2)
            if len(results) == len(tickers):
                return results
        except Exception as e:
            print(f"[MARKET WARN] yfinance live fetch error: {e}")
        return {}
