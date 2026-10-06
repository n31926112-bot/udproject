/**
 * TradeList.h - Hand-written singly linked list for storing Trade objects.
 * Demonstrates: Dynamic memory allocation (new/delete), head and tail pointers (O(1) append),
 * rule of five (copy/move control), custom list traversal.
 */
#ifndef TRADE_LIST_H
#define TRADE_LIST_H

#include "Trade.h"
#include <cstddef>
#include <functional>

struct TradeNode {
    Trade trade;
    TradeNode* next;

    explicit TradeNode(const Trade& t) : trade(t), next(nullptr) {}
    explicit TradeNode(Trade&& t) : trade(std::move(t)), next(nullptr) {}
};

class TradeList {
private:
    TradeNode* m_head;
    TradeNode* m_tail;
    size_t m_size;

public:
    TradeList();
    ~TradeList();

    // Rule of five: disable copy construction and copy assignment to enforce sole ownership
    TradeList(const TradeList&) = delete;
    TradeList& operator=(const TradeList&) = delete;

    // Support move operations
    TradeList(TradeList&& other) noexcept;
    TradeList& operator=(TradeList&& other) noexcept;

    void append(const Trade& trade);
    void insertSorted(const Trade& trade); // chronological order by date string (YYYY-MM-DD)

    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    void clear();

    void forEach(const std::function<void(const Trade&)>& callback) const;

    // Expose head for iteration if needed
    const TradeNode* getHead() const { return m_head; }
};

#endif // TRADE_LIST_H
