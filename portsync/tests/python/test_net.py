"""
test_net.py - Unit tests for NetClient connection retry and send validation.
"""
import unittest
import socket
import threading
import time
from python.net_client import NetClient
from python.protocol import encode_hello, encode_bye

class TestNetClient(unittest.TestCase):
    def test_connect_failure_and_retry(self):
        # Unused port
        client = NetClient(port=59999, max_retries=2, retry_delay=0.1)
        res = client.connect()
        self.assertFalse(res)

    def test_mock_server_connection(self):
        server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_sock.bind(("127.0.0.1", 0))
        port = server_sock.getsockname()[1]
        server_sock.listen(1)

        received_lines = []

        def mock_server():
            conn, _ = server_sock.accept()
            data = conn.recv(1024).decode("utf-8")
            received_lines.append(data)
            conn.close()
            server_sock.close()

        t = threading.Thread(target=mock_server)
        t.start()

        client = NetClient(port=port, max_retries=3, retry_delay=0.1)
        self.assertTrue(client.connect())
        client.send_line(encode_hello("TestApp", 1))
        client.close()

        t.join()
        self.assertEqual(len(received_lines), 1)
        self.assertIn("HELLO|TestApp|1", received_lines[0])

if __name__ == "__main__":
    unittest.main()
