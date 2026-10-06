"""
protocol.py - Wire protocol definition and validation for PortSync IPC.
Demonstrates: Clean boundary validation, serialization/deserialization helper.
"""

from typing import Dict, Any, Optional

def sanitize_text(text: str) -> str:
    """Replace pipe and newline characters with spaces to adhere to protocol wire rules."""
    return text.replace('|', ' ').replace('\n', ' ').replace('\r', ' ').strip()

def encode_hello(app_name: str = "PortSync", version: int = 1) -> str:
    return f"HELLO|{sanitize_text(app_name)}|{version}\n"

def encode_trade(broker: str, ticker: str, side: str, qty: int, price: float, date_str: str) -> str:
    assert side in ("BUY", "SELL"), f"Invalid trade side: {side}"
    assert qty > 0, f"Quantity must be positive: {qty}"
    assert price > 0, f"Price must be positive: {price}"
    return f"TRADE|{sanitize_text(broker)}|{sanitize_text(ticker).upper()}|{side}|{qty}|{price:.2f}|{sanitize_text(date_str)}\n"

def encode_price(ticker: str, price: float, time_str: str) -> str:
    assert price > 0, f"Price must be positive: {price}"
    return f"PRICE|{sanitize_text(ticker).upper()}|{price:.2f}|{sanitize_text(time_str)}\n"

def encode_hist(ticker: str, date_str: str, close: float) -> str:
    assert close > 0, f"Close price must be positive: {close}"
    return f"HIST|{sanitize_text(ticker).upper()}|{sanitize_text(date_str)}|{close:.2f}\n"

def encode_news(ticker: str, date_str: str, headline: str) -> str:
    return f"NEWS|{sanitize_text(ticker).upper()}|{sanitize_text(date_str)}|{sanitize_text(headline)}\n"

def encode_batch_end(seq: int) -> str:
    return f"BATCH_END|{seq}\n"

def encode_bye() -> str:
    return "BYE\n"

def validate_line(line: str) -> bool:
    """Validate a wire protocol line before sending."""
    if not line.endswith("\n"):
        return False
    parts = line.rstrip("\r\n").split("|")
    if not parts:
        return False
    rtype = parts[0]
    if rtype == "HELLO":
        return len(parts) == 3 and parts[2].isdigit()
    elif rtype == "TRADE":
        # TRADE|<broker>|<ticker>|<BUY|SELL>|<qty:int>|<price:double>|<YYYY-MM-DD>
        if len(parts) != 7: return False
        if parts[3] not in ("BUY", "SELL"): return False
        try:
            q = int(parts[4])
            p = float(parts[5])
            if q <= 0 or p <= 0: return False
        except ValueError:
            return False
        return True
    elif rtype == "PRICE":
        # PRICE|<ticker>|<price:double>|<HH:MM:SS>
        if len(parts) != 4: return False
        try:
            if float(parts[2]) <= 0: return False
        except ValueError:
            return False
        return True
    elif rtype == "HIST":
        # HIST|<ticker>|<YYYY-MM-DD>|<close:double>
        if len(parts) != 4: return False
        try:
            if float(parts[3]) <= 0: return False
        except ValueError:
            return False
        return True
    elif rtype == "NEWS":
        # NEWS|<ticker>|<YYYY-MM-DD>|<headline>
        return len(parts) >= 4
    elif rtype == "BATCH_END":
        return len(parts) == 2 and parts[1].isdigit()
    elif rtype == "BYE":
        return len(parts) == 1
    return False
