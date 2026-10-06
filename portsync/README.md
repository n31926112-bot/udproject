# PortSync — Cross-Broker Portfolio Aggregation System

**University Project**: Data Structures with C (TCS302) & Object-Oriented Programming with C++ (TCS307)  
**Team**: Avengers (3 members)

PortSync is a cross-broker portfolio aggregation system designed to collect, stream, and aggregate trade positions and live market prices across multiple brokerages (e.g., Groww and Zerodha). It features a Python telemetry collector streaming via a TCP socket interface to a high-performance C++ core engine built with custom hand-written data structures.

---

## 🏗 System Architecture

```text
  +-------------------------------------------------------------------+
  |                          PYTHON COLLECTOR                         |
  |  +--------------------+  +--------------------+  +-------------+  |
  |  | GrowwSampleAdapter |  |ZerodhaSampleAdapter|  | MarketEngine|  |
  |  +---------+----------+  +---------+----------+  +------+------+  |
  |            |                       |                    |         |
  |            +-----------+-----------+                    |         |
  |                        |                                |         |
  |                 Protocol Encoder                        |         |
  |                        |                                |         |
  +------------------------|--------------------------------|---------+
                           | Wire Protocol (TCP:5050)       |
                           v                                v
  +-------------------------------------------------------------------+
  |                           C++ SERVER CORE                         |
  |  +-------------------------------------------------------------+  |
  |  | SocketServer (Line-Buffered TCP Socket Receiver)            |  |
  |  +------------------------------+------------------------------+  |
  |                                 |                                 |
  |                          stringstream Parser                      |
  |                                 |                                 |
  |                        PortfolioManager                           |
  |                     /                     \                       |
  |          Hand-written Singly             Hand-written AVL         |
  |              TradeList                      TickerTree            |
  |          (Head/Tail O(1))             (O(log n) Rotations)        |
  |                                                                   |
  |  +-------------------------------------------------------------+  |
  |  | Dashboard CLI (Polymorphic Virtual Dispatch & Terminal UI)  |  |
  |  +-------------------------------------------------------------+  |
  +-------------------------------------------------------------------+
```

---

## ⚡ Key Features & Academic Requirements

- **C++ Core Engine (C++17)**: Built with zero external third-party dependencies.
- **Hand-Written Data Structures**:
  - **Singly Linked List (`TradeList`)**: Features head and tail pointers for $O(1)$ appends and `insertSorted` for chronological order.
  - **Self-Balancing Binary Search Tree (`TickerTree` / AVL)**: Hand-crafted with LL, RR, LR, and RL rotations maintaining $O(\log n)$ operations.
- **OOP Architecture**:
  - Abstract base class `Printable` demonstrating **polymorphism** and **virtual method dispatch** in `Dashboard`.
  - Strict **encapsulation**, **const-correctness**, and zero global mutable state.
  - **RAII & Copy Control**: Enforces sole ownership via Rule of Five (`delete` copy operations) and leak-free dynamic memory management validated via AddressSanitizer/UBSan.
- **Cross-Platform IPC**: Line-buffered TCP socket server using `#ifdef _WIN32` for Winsock2 (`-lws2_32`) and POSIX sockets for Linux/macOS.
- **Python Telemetry Collector**: Modular adapter pattern with sample CSV ingestion and optional live integration stubs (`yfinance`, Zerodha Kite).
- **Honest Labelling**: Standardized 1-year estimates explicitly marked as **`ESTIMATE ONLY - NOT A PREDICTION`**, and system polling marked as **`near-real-time`**.

---

## 🚀 Build & Run Instructions

### Prerequisites
- `g++` supporting C++17
- `make`
- `python3` (3.10+)

### 1. Build C++ Engine & Run Tests
```bash
make            # Builds portsync binary and test executables
make test       # Runs all C++ unit test suites (TradeList, TickerTree AVL, Parser, Portfolio, Projection)
make sanitizer  # Builds and runs test suite under AddressSanitizer & UndefinedBehaviorSanitizer
```

### 2. Run Sample Mode Demo
In Terminal 1 (Start C++ Server):
```bash
./portsync --port 5050
```

In Terminal 2 (Launch Python Collector):
```bash
python3 python/collector.py --port 5050 --mode sample --interval 5
```

### 3. Run End-to-End Automated Test
```bash
./tests/e2e.sh
```

---

## 📌 Limitations
1. **Groww API**: Groww does not provide a public trade API; Groww integration uses CSV import by design.
2. **Near-Real-Time Streaming**: Streamed data updates at configured polling intervals (default: 5 seconds).
3. **One-Year Projections**: Projections use historical CAGR / linear regression trend models and represent estimated approximations, not guaranteed future predictions.
