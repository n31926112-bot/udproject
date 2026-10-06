"""
test_brokers.py - Unit tests for broker adapters.
"""
import unittest
import os
from python.brokers.groww_sample import GrowwSampleAdapter
from python.brokers.zerodha_sample import ZerodhaSampleAdapter
from python.brokers.zerodha_kite import ZerodhaKiteAdapter

class TestBrokers(unittest.TestCase):
    def test_groww_sample(self):
        adapter = GrowwSampleAdapter()
        trades = adapter.get_trades()
        self.assertGreater(len(trades), 0)
        self.assertEqual(trades[0].broker, "Groww")

    def test_zerodha_sample(self):
        adapter = ZerodhaSampleAdapter()
        trades = adapter.get_trades()
        self.assertGreater(len(trades), 0)
        self.assertEqual(trades[0].broker, "Zerodha")

    def test_shared_tickers(self):
        g_trades = GrowwSampleAdapter().get_trades()
        z_trades = ZerodhaSampleAdapter().get_trades()
        g_tickers = {t.ticker for t in g_trades}
        z_tickers = {t.ticker for t in z_trades}
        shared = g_tickers.intersection(z_tickers)
        self.assertGreaterEqual(len(shared), 3, "Must have at least 3 tickers in both brokers")

    def test_zerodha_kite_missing_env(self):
        # Ensure env vars are absent
        old_key = os.environ.pop("KITE_API_KEY", None)
        old_secret = os.environ.pop("KITE_ACCESS_TOKEN", None)
        try:
            with self.assertRaises(RuntimeError) as ctx:
                ZerodhaKiteAdapter()
            self.assertIn("missing", str(ctx.exception).lower())
        finally:
            if old_key: os.environ["KITE_API_KEY"] = old_key
            if old_secret: os.environ["KITE_ACCESS_TOKEN"] = old_secret

if __name__ == "__main__":
    unittest.main()
