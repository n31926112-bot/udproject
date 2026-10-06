"""
zerodha_kite.py - Stub for live Zerodha Kite Connect API integration.
Requires KITE_API_KEY and KITE_ACCESS_TOKEN environment variables.
"""
import os
from typing import List
from .base import BaseBrokerAdapter, TradeRecord

class ZerodhaKiteAdapter(BaseBrokerAdapter):
    def __init__(self):
        self.api_key = os.getenv("KITE_API_KEY")
        self.access_token = os.getenv("KITE_ACCESS_TOKEN")
        
        if not self.api_key or not self.access_token:
            raise RuntimeError(
                "Zerodha Kite API credentials missing! Please set environment variables "
                "'KITE_API_KEY' and 'KITE_ACCESS_TOKEN' to enable live Kite integration."
            )

    def get_trades(self) -> List[TradeRecord]:
        # Lazily import kiteconnect if available
        try:
            from kiteconnect import KiteConnect
        except ImportError as e:
            raise RuntimeError(
                "The 'kiteconnect' Python package is not installed. "
                "Install it via 'pip install kiteconnect' to use ZerodhaKiteAdapter."
            ) from e

        kite = KiteConnect(api_key=self.api_key)
        kite.set_access_token(self.access_token)

        raw_trades = kite.trades()
        trades = []
        for t in raw_trades:
            trades.append(TradeRecord(
                broker="Zerodha",
                ticker=t.get("tradingsymbol", ""),
                side=t.get("transaction_type", "BUY"),
                qty=int(t.get("quantity", 0)),
                price=float(t.get("average_price", 0.0)),
                date_str=str(t.get("order_timestamp", ""))[:10]
            ))
        return trades
