/**
 * test_list.cpp - Unit tests for hand-written singly linked list TradeList.
 */
#include "TradeList.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

void testAppendAndOrder() {
    TradeList list;
    assert(list.size() == 0);
    assert(list.empty());
    assert(list.getHead() == nullptr);

    list.append(Trade("Groww", "RELIANCE", TradeSide::BUY, 10, 2400.0, "2023-01-10"));
    assert(list.getHead() != nullptr);
    assert(list.getHead()->trade.getTicker() == "RELIANCE");

    list.append(Trade("Zerodha", "TCS", TradeSide::BUY, 5, 3200.0, "2023-01-12"));
    list.append(Trade("Groww", "INFY", TradeSide::BUY, 15, 1400.0, "2023-01-15"));

    assert(list.size() == 3);

    std::vector<std::string> symbols;
    list.forEach([&symbols](const Trade& t) {
        symbols.push_back(t.getTicker());
    });

    assert(symbols.size() == 3);
    assert(symbols[0] == "RELIANCE");
    assert(symbols[1] == "TCS");
    assert(symbols[2] == "INFY");
    std::cout << "[PASS] testAppendAndOrder\n";
}

void testInsertSortedHeadMiddleTailAndStability() {
    TradeList list;

    // 1. Insert in middle initially (first element)
    Trade tMiddle(Trade("Groww", "INFY", TradeSide::BUY, 10, 1400.0, "2023-05-10"));
    list.insertSorted(tMiddle);
    assert(list.size() == 1);
    assert(list.getHead()->trade.getTicker() == "INFY");
    // Check tail pointer
    const TradeNode* curr = list.getHead();
    while (curr->next != nullptr) curr = curr->next;
    assert(curr->trade.getTicker() == "INFY");

    // 2. Insert at Head (date earlier than head)
    Trade tHead(Trade("Zerodha", "RELIANCE", TradeSide::BUY, 20, 2400.0, "2023-01-01"));
    list.insertSorted(tHead);
    assert(list.size() == 2);
    assert(list.getHead()->trade.getTicker() == "RELIANCE");
    // Verify tail pointer is still INFY
    curr = list.getHead();
    while (curr->next != nullptr) curr = curr->next;
    assert(curr->trade.getTicker() == "INFY");

    // 3. Insert at Tail (date later than tail)
    Trade tTail(Trade("Groww", "TCS", TradeSide::BUY, 5, 3200.0, "2023-09-20"));
    list.insertSorted(tTail);
    assert(list.size() == 3);
    curr = list.getHead();
    while (curr->next != nullptr) curr = curr->next;
    assert(curr->trade.getTicker() == "TCS");

    // 4. Insert in Middle (date between RELIANCE and INFY)
    Trade tMid2(Trade("Zerodha", "HDFCBANK", TradeSide::BUY, 15, 1500.0, "2023-03-15"));
    list.insertSorted(tMid2);
    assert(list.size() == 4);
    curr = list.getHead();
    while (curr->next != nullptr) curr = curr->next;
    assert(curr->trade.getTicker() == "TCS");

    // 5. Test Equal Date Stability (Equal date insert must be placed AFTER existing equal date)
    Trade tEq1(Trade("Groww", "WIPRO", TradeSide::BUY, 30, 400.0, "2023-03-15"));
    list.insertSorted(tEq1);
    assert(list.size() == 5);

    std::vector<std::string> dates;
    std::vector<std::string> symbols;
    list.forEach([&dates, &symbols](const Trade& t) {
        dates.push_back(t.getDate());
        symbols.push_back(t.getTicker());
    });

    assert(symbols[0] == "RELIANCE"); // 2023-01-01
    assert(symbols[1] == "HDFCBANK"); // 2023-03-15 (First)
    assert(symbols[2] == "WIPRO");    // 2023-03-15 (Second - Stable insertion after HDFCBANK)
    assert(symbols[3] == "INFY");     // 2023-05-10
    assert(symbols[4] == "TCS");      // 2023-09-20

    std::cout << "[PASS] testInsertSortedHeadMiddleTailAndStability\n";
}

void testClearAndDestructor() {
    TradeList list;
    for (int i = 0; i < 100; ++i) {
        list.append(Trade("Broker", "TICKER", TradeSide::BUY, 1, 100.0, "2023-01-01"));
    }
    assert(list.size() == 100);
    list.clear();
    assert(list.size() == 0);
    assert(list.empty());
    assert(list.getHead() == nullptr);
    std::cout << "[PASS] testClearAndDestructor\n";
}

int main() {
    std::cout << "--- Running TradeList Unit Tests ---\n";
    testAppendAndOrder();
    testInsertSortedHeadMiddleTailAndStability();
    testClearAndDestructor();
    std::cout << "All TradeList tests passed successfully!\n";
    return 0;
}
