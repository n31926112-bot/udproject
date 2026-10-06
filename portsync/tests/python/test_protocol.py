"""
test_protocol.py - Unit tests for Python wire protocol encoders and validators.
"""
import unittest
from python.protocol import (
    encode_hello, encode_trade, encode_price, encode_hist,
    encode_news, encode_batch_end, encode_bye, validate_line
)

class TestProtocol(unittest.TestCase):
    def test_hello(self):
        line = encode_hello("PortSync", 1)
        self.assertEqual(line, "HELLO|PortSync|1\n")
        self.assertTrue(validate_line(line))

    def test_trade(self):
        line = encode_trade("Groww", "RELIANCE", "BUY", 10, 2450.5, "2023-01-15")
        self.assertEqual(line, "TRADE|Groww|RELIANCE|BUY|10|2450.50|2023-01-15\n")
        self.assertTrue(validate_line(line))

    def test_price(self):
        line = encode_price("TCS", 3300.0, "10:00:00")
        self.assertEqual(line, "PRICE|TCS|3300.00|10:00:00\n")
        self.assertTrue(validate_line(line))

    def test_news_sanitization(self):
        line = encode_news("INFY", "2023-09-20", "Profit up | Q2 results\nNew era")
        self.assertNotIn("| Q2", line.split("|")[3])
        self.assertTrue(validate_line(line))

    def test_batch_end_and_bye(self):
        be = encode_batch_end(5)
        self.assertTrue(validate_line(be))
        bye = encode_bye()
        self.assertTrue(validate_line(bye))

    def test_invalid_lines(self):
        self.assertFalse(validate_line("TRADE|Groww|RELIANCE|INVALID|10|2450.50|2023-01-15\n"))
        self.assertFalse(validate_line("TRADE|Groww|RELIANCE|BUY|-5|2450.50|2023-01-15\n"))
        self.assertFalse(validate_line("HELLO|PortSync\n"))

if __name__ == "__main__":
    unittest.main()
