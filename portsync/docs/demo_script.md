# PortSync 5-Minute Viva / Demo Walkthrough Script

Use this script during professor evaluation or viva presentations.

---

## 🕒 Timeline & Key Talking Points

### Minute 1: System Overview & Architecture
1. **Show Architecture Diagram**: Open `README.md` or `docs/architecture.md`.
2. **Explain Role Split**:
   - *Python Collector*: Ingests trade CSVs/APIs, market prices, and news, encoding them into a clean pipe-delimited wire protocol (`protocol.py`).
   - *C++ Server*: Listens on local TCP socket `127.0.0.1:5050`, parses line-buffered socket streams using `std::stringstream`, updates custom data structures, and renders the CLI dashboard.

---

### Minute 2: Hand-Written Data Structures (TCS302)
1. **Singly Linked List (`TradeList.h / TradeList.cpp`)**:
   - Point out **Head & Tail Pointers** (`m_head`, `m_tail` in `TradeList.h`). Explain how storing `m_tail` achieves **$O(1)$ constant time append**.
   - Show `insertSorted()` in `TradeList.cpp` to demonstrate chronological sorting of interleaved broker trade streams.
2. **Self-Balancing AVL Tree (`TickerTree.h / TickerTree.cpp`)**:
   - Explain why AVL was chosen over plain BST: alphabetical ticker sequences (e.g., `HDFCBANK` -> `INFY` -> `ITC` -> `RELIANCE` -> `TCS`) cause plain BSTs to degenerate into $O(N)$ lists. AVL guarantees $h \le 1.44 \log_2 N$, giving **$O(\log N)$ operations**.
   - Show tree rotations (`rotateRight`, `rotateLeft`) in `TickerTree.cpp`.

---

### Minute 3: OOP Concepts & Memory Management (TCS307)
1. **Abstract Base Class & Polymorphism**:
   - Open `Printable.h`. Show `virtual std::string toString() const = 0;`.
   - Point out `Trade`, `Holding`, and `NewsItem` inheriting from `Printable`.
   - Open `Dashboard.cpp` and show `renderPolymorphicSummary(const std::vector<const Printable*>& items)` demonstrating **runtime virtual method dispatch**.
2. **RAII & Copy Control (Rule of Five)**:
   - Show deleted copy constructor and assignment operator in `TradeList.h` and `TickerTree.h` (`TradeList(const TradeList&) = delete;`).
   - Show recursive destructor `freeTree()` in `TickerTree.cpp`.

---

### Minute 4: Live Demo Execution
Open two terminal windows:

**Terminal 1 (Start C++ Server)**:
```bash
./portsync --port 5050
```

**Terminal 2 (Start Python Collector)**:
```bash
python3 python/collector.py --port 5050 --mode sample --interval 5
```

**What to point out on screen**:
- Handshake `HELLO|PortSync|1` in C++ log.
- Unified holdings table merging Groww and Zerodha positions (e.g., `RELIANCE`, `TCS`, `INFY`).
- Weighted average cost and realized P/L math.
- Periodic near-real-time price updates re-rendering the dashboard.
- Honest disclaimer label: **`1-YEAR ESTIMATED PROJECTIONS (Disclaimer: ESTIMATE ONLY - NOT A PREDICTION)`**.

---

### Minute 5: Verification & Automated Tests
Execute unit test suite and sanitizer:
```bash
make sanitizer && ./bin/test_list && ./bin/test_avl && ./bin/test_parser && ./bin/test_portfolio && ./bin/test_projection
```
Execute E2E Integration test:
```bash
./tests/e2e.sh
```
Highlight: Zero memory leaks, zero buffer overflows, zero warnings (`-Wall -Wextra`).
