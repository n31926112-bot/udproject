"""
base.py - Abstract base class for broker adapters.
"""
from abc import ABC, abstractmethod
from typing import List, Dict, Any

class TradeRecord:
    def __init__(self, broker: str, ticker: str, side: str, qty: int, price: float, date_str: str):
        self.broker = broker
        self.ticker = ticker
        self.side = side
        self.qty = qty
        self.price = price
        self.date_str = date_str

class BaseBrokerAdapter(ABC):
    @abstractmethod
    def get_trades(self) -> List[TradeRecord]:
        """Fetch trades from broker source."""
        pass
