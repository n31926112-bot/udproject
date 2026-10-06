/**
 * TradeList.cpp - Implementation of singly linked list for trades.
 */
#include "TradeList.h"
#include <utility>

TradeList::TradeList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

TradeList::~TradeList() {
    clear();
}

TradeList::TradeList(TradeList&& other) noexcept
    : m_head(other.m_head), m_tail(other.m_tail), m_size(other.m_size) {
    other.m_head = nullptr;
    other.m_tail = nullptr;
    other.m_size = 0;
}

TradeList& TradeList::operator=(TradeList&& other) noexcept {
    if (this != &other) {
        clear();
        m_head = other.m_head;
        m_tail = other.m_tail;
        m_size = other.m_size;
        other.m_head = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
    }
    return *this;
}

void TradeList::clear() {
    TradeNode* current = m_head;
    while (current != nullptr) {
        TradeNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    m_head = nullptr;
    m_tail = nullptr;
    m_size = 0;
}

void TradeList::append(const Trade& trade) {
    TradeNode* newNode = new TradeNode(trade);
    if (m_head == nullptr) {
        m_head = newNode;
        m_tail = newNode;
    } else {
        m_tail->next = newNode;
        m_tail = newNode;
    }
    m_size++;
}

void TradeList::insertSorted(const Trade& trade) {
    TradeNode* newNode = new TradeNode(trade);
    
    // Case 1: Empty list or belongs before head
    if (m_head == nullptr || trade.getDate() < m_head->trade.getDate()) {
        newNode->next = m_head;
        m_head = newNode;
        if (m_tail == nullptr) {
            m_tail = newNode;
        }
        m_size++;
        return;
    }

    // Case 2: Traverse to find correct insertion point
    TradeNode* current = m_head;
    while (current->next != nullptr && current->next->trade.getDate() <= trade.getDate()) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;

    // Update tail if inserted at the end
    if (newNode->next == nullptr) {
        m_tail = newNode;
    }
    m_size++;
}

void TradeList::forEach(const std::function<void(const Trade&)>& callback) const {
    TradeNode* current = m_head;
    while (current != nullptr) {
        callback(current->trade);
        current = current->next;
    }
}
