"""
net_client.py - Robust TCP client with reconnection logic and sendall.
"""
import socket
import time
from typing import Optional
try:
    from .protocol import validate_line
except ImportError:
    from protocol import validate_line

class NetClient:
    def __init__(self, host: str = "127.0.0.1", port: int = 5050, max_retries: int = 10, retry_delay: float = 1.0):
        self.host = host
        self.port = port
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        self.sock: Optional[socket.socket] = None

    def connect(self) -> bool:
        """Attempt to connect to C++ server with retries."""
        for attempt in range(1, self.max_retries + 1):
            try:
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock.connect((self.host, self.port))
                print(f"[CLIENT] Connected to C++ server at {self.host}:{self.port}")
                return True
            except (ConnectionRefusedError, OSError) as e:
                if self.sock:
                    self.sock.close()
                    self.sock = None
                print(f"[CLIENT] Connection attempt {attempt}/{self.max_retries} failed ({e}). Retrying in {self.retry_delay}s...")
                time.sleep(self.retry_delay)
        return False

    def send_line(self, line: str) -> bool:
        if not self.sock:
            raise RuntimeError("Client not connected")
        if not validate_line(line):
            print(f"[CLIENT WARN] Refusing to send invalid protocol line: {line.strip()}")
            return False
        try:
            self.sock.sendall(line.encode("utf-8"))
            return True
        except OSError as e:
            print(f"[CLIENT ERROR] Failed to send line: {e}")
            return False

    def close(self):
        if self.sock:
            try:
                self.sock.close()
            except OSError:
                pass
            self.sock = None
            print("[CLIENT] Socket closed.")
