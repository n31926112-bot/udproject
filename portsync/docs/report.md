# PortSync: Cross-Broker Portfolio Aggregation System

**Course**: Data Structures with C (TCS302) & Object-Oriented Programming with C++ (TCS307)  
**Team**: Avengers (3 members)  
**Date**: Academic Year 2026  

---

## 1. Introduction
Modern retail investors frequently maintain trading accounts across multiple discount brokerages (such as Groww and Zerodha) to access distinct platform features or diversify operational risk. However, tracking overall financial health across fragmented accounts is cumbersome. PortSync is a real-time cross-broker portfolio aggregation system designed to ingest, aggregate, and evaluate multi-broker equity positions and price telemetry via high-performance hand-written data structures.

---

## 2. Problem Statement
Aggregating fragmented trade records presents two primary computational challenges:
1. **Unordered / Interleaved Trade Ingestion**: Trades received from multiple brokers arrive out of order or interleaved.
2. **Fast Ticker Lookup & Re-aggregation**: Real-time price updates require rapid lookups to recalculate unrealized profit/loss (P/L) across all held positions.

PortSync solves these problems without relying on standard library containers (`std::list`, `std::map`, `std::set`, `std::unordered_map`) in its core data path, meeting strict university grading requirements.

---

## 3. Data Structure Justifications

### 3.1 Hand-Written Singly Linked List with Head & Tail Pointers (`TradeList`)
- **Why Linked List?**: Trade logs represent sequential transaction streams where trades are appended continuously.
- **Why Head AND Tail Pointers?**: A traditional singly linked list requires $O(N)$ traversal to append a node at the tail. By storing an explicit `m_tail` pointer alongside `m_head`, `append()` executes in **$O(1)$ constant time**.
- **Chronological Sorting (`insertSorted`)**: Enables maintaining trade history in chronological date order ($O(N)$ worst case for insertion, stable for equal dates).

### 3.2 Hand-Written Self-Balancing BST (`TickerTree` / AVL Tree)
- **Why AVL Tree over Plain BST?**: In financial telemetry, stock ticker symbols are often ingested in sorted alphabetical order (e.g., `HDFCBANK`, `INFY`, `ITC`, `RELIANCE`, `SBIN`, `TATAMOTORS`, `TCS`, `WIPRO`). A plain Binary Search Tree (BST) degrades into a linear linked list under sorted insertion, increasing lookup time to $O(N)$. An AVL tree guarantees a strictly balanced height of $h \le 1.44 \log_2(N)$, guaranteeing **$O(\log N)$ search, insertion, and deletion times**.
- **Rotations**: Self-balancing is maintained via four fundamental rotations: Left-Left (Single Right), Right-Right (Single Left), Left-Right, and Right-Left.

| Data Structure | Operation | Average Complexity | Worst-Case Complexity |
|----------------|-----------|--------------------|-----------------------|
| **TradeList**  | Append    | $O(1)$             | $O(1)$                |
| **TradeList**  | Traverse  | $O(N)$             | $O(N)$                |
| **TickerTree** | Search    | $O(\log N)$        | $O(\log N)$           |
| **TickerTree** | Insert    | $O(\log N)$        | $O(\log N)$           |
| **TickerTree** | Delete    | $O(\log N)$        | $O(\log N)$           |

---

## 4. Object-Oriented Programming (OOP) Mapping

| OOP Concept | Implementation Class | Description / Demonstration |
|-------------|----------------------|-----------------------------|
| **Abstraction & Polymorphism** | `Printable.h` | Abstract base class declaring pure virtual function `virtual std::string toString() const = 0`. Inherited by `Trade`, `Holding`, and `NewsItem`. |
| **Virtual Dispatch** | `Dashboard.cpp` | Iterates over heterogeneous arrays of `const Printable*` pointers and invokes `item->toString()` at runtime. |
| **Encapsulation** | `Trade.h`, `Holding.h` | Private member fields exposed exclusively through const getter methods and audited mutator methods (`applyBuy`, `applySell`). |
| **RAII & Copy Control** | `TradeList.h`, `TickerTree.h` | Explicit deletion of copy constructors/assignments (`= delete`), custom move constructors, and deterministic destructor cleanup. |
| **Const-Correctness** | All C++ Classes | All read-only methods marked `const`; parameters passed via `const std::string&`. |

---

## 5. Memory Management & Safety Strategy
- **Raw Pointers with `new` / `delete`**: Dynamic allocation of nodes without smart pointers to satisfy syllabus rules.
- **Rule of Five**: Containers delete copy operations to eliminate accidental shallow copies and double-free vulnerabilities.
- **Verification**: Zero memory leaks or undefined behavior confirmed using AddressSanitizer (`-fsanitize=address,undefined`).

---

## 6. Testing & Verification
The project incorporates a 4-tier testing strategy:
1. **C++ Unit Tests** (`make test`): Covers `test_list.cpp`, `test_avl.cpp`, `test_parser.cpp`, `test_portfolio.cpp`, `test_projection.cpp`.
2. **Python Unit Tests** (`python3 -m unittest`): Tests protocol encoding/decoding, CSV loaders, and network client retries.
3. **Sample Data Validator** (`python/validate_sample_data.py`): Verifies dataset integrity (3+ shared tickers, zero overselling, alphabetical sequence, ~250 historical price records).
4. **End-to-End Test** (`tests/e2e.sh`): Executes server and collector concurrently in sample mode, verifying socket stream processing and dashboard output.

---

## 7. Limitations & Future Scope
- **Limitations**: Groww adapter operates via CSV import due to lack of public Groww API. One-year projections are estimated approximations based on historical CAGR / linear regression trend.
- **Future Scope**: Integration of WebSocket feeds for sub-second streaming and persistent database storage.
