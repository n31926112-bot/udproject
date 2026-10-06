"""
zerodha_sample.py - Zerodha sample CSV broker adapter.
"""
import csv
import os
from typing import List
from .base import BaseBrokerAdapter, TradeRecord

class ZerodhaSampleAdapter(BaseBrokerAdapter):
    def __init__(self, csv_path: str = None):
        if csv_path is None:
            base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            csv_path = os.path.join(base_dir, "sample_data", "zerodha_trades.csv")
        self.csv_path = csv_path

    def get_trades(self) -> List[TradeRecord]:
        trades = []
        if not os.path.exists(self.csv_path):
            return trades
        with open(self.csv_path, mode="r", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                trades.append(TradeRecord(
                    broker=row["broker"],
                    ticker=row["ticker"],
                    side=row["side"],
                    qty=int(row["qty"]),
                    price=float(row["price"]),
                    date_str=row["date"]
                ))
        return trades
