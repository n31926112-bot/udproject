"""
news.py - News provider supporting sample headlines and optional live news fetch.
"""
import os
import json
from typing import Dict, List, Tuple

class NewsEngine:
    def __init__(self, sample_json_path: str = None):
        if sample_json_path is None:
            base_dir = os.path.dirname(os.path.abspath(__file__))
            sample_json_path = os.path.join(base_dir, "sample_data", "news.json")
        self.sample_json_path = sample_json_path
        self.news_data: Dict[str, List[Dict[str, str]]] = {}
        self._load_news()

    def _load_news(self):
        if os.path.exists(self.sample_json_path):
            with open(self.sample_json_path, "r", encoding="utf-8") as f:
                self.news_data = json.load(f)

    def get_news(self, mode: str = "sample") -> List[Tuple[str, str, str]]:
        """Returns list of tuples: (ticker, date_str, headline)"""
        items = []
        for ticker, headlines in self.news_data.items():
            for h in headlines:
                items.append((ticker, h.get("date", "2023-10-01"), h.get("headline", "")))
        return items
