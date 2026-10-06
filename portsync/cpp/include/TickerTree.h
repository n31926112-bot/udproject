/**
 * TickerTree.h - Hand-written self-balancing AVL binary search tree for TickerNode.
 * Demonstrates: Balanced BST algorithms (O(log n) lookups & insertions), tree rotations,
 * recursive node cleanup (RAII), copy restriction (rule of five).
 */
#ifndef TICKER_TREE_H
#define TICKER_TREE_H

#include "Ticker.h"
#include <string>
#include <cstddef>
#include <functional>

class TickerTree {
private:
    TickerNode* m_root;
    size_t m_size;

    // Helper functions for AVL self-balancing
    int getHeight(TickerNode* node) const;
    int getBalanceFactor(TickerNode* node) const;
    void updateHeight(TickerNode* node);

    TickerNode* rotateRight(TickerNode* y);
    TickerNode* rotateLeft(TickerNode* x);

    TickerNode* insertNode(TickerNode* node, const std::string& symbol, TickerNode** insertedOrFoundNode);
    TickerNode* findNode(TickerNode* node, const std::string& symbol) const;
    TickerNode* findMin(TickerNode* node) const;
    TickerNode* removeNode(TickerNode* node, const std::string& symbol);
    void freeTree(TickerNode* node);
    void inorderHelper(TickerNode* node, const std::function<void(TickerNode*)>& callback) const;

public:
    TickerTree();
    ~TickerTree();

    // Disable copy constructors/assignment (Rule of 5)
    TickerTree(const TickerTree&) = delete;
    TickerTree& operator=(const TickerTree&) = delete;

    TickerTree(TickerTree&& other) noexcept;
    TickerTree& operator=(TickerTree&& other) noexcept;

    TickerNode* insert(const std::string& symbol); // inserts or returns existing node
    TickerNode* find(const std::string& symbol) const;
    bool remove(const std::string& symbol);

    TickerNode* getRoot() const { return m_root; }
    size_t size() const { return m_size; }
    int height() const { return getHeight(m_root); }
    bool empty() const { return m_root == nullptr; }

    void inorder(const std::function<void(TickerNode*)>& callback) const;
    void clear();
};

#endif // TICKER_TREE_H
