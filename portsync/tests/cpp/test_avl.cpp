/**
 * test_avl.cpp - Unit tests for hand-written self-balancing AVL TickerTree.
 */
#include "TickerTree.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

// Helper functions to check AVL invariants recursively
static int checkHeightAndBalance(TickerNode* node) {
    if (node == nullptr) return 0;
    int leftH = checkHeightAndBalance(node->left);
    int rightH = checkHeightAndBalance(node->right);
    int bf = leftH - rightH;
    assert(bf >= -1 && bf <= 1);
    int expectedH = 1 + std::max(leftH, rightH);
    assert(node->height == expectedH);
    return expectedH;
}



void testRotationsIndividually() {
    // 1. LL Rotation (Inserting in Left child of Left subtree: C, B, A)
    {
        TickerTree tree;
        tree.insert("C");
        tree.insert("B");
        tree.insert("A"); // Triggers LL rotation (rotateRight on C)
        assert(tree.height() == 2);
        assert(tree.find("B") != nullptr);
        std::vector<std::string> syms;
        tree.inorder([&syms](TickerNode* n) { syms.push_back(n->symbol); });
        assert(syms == std::vector<std::string>({"A", "B", "C"}));
    }

    // 2. RR Rotation (Inserting in Right child of Right subtree: A, B, C)
    {
        TickerTree tree;
        tree.insert("A");
        tree.insert("B");
        tree.insert("C"); // Triggers RR rotation (rotateLeft on A)
        assert(tree.height() == 2);
        std::vector<std::string> syms;
        tree.inorder([&syms](TickerNode* n) { syms.push_back(n->symbol); });
        assert(syms == std::vector<std::string>({"A", "B", "C"}));
    }

    // 3. LR Rotation (Inserting in Right child of Left subtree: C, A, B)
    {
        TickerTree tree;
        tree.insert("C");
        tree.insert("A");
        tree.insert("B"); // Triggers LR rotation (rotateLeft on A then rotateRight on C)
        assert(tree.height() == 2);
        std::vector<std::string> syms;
        tree.inorder([&syms](TickerNode* n) { syms.push_back(n->symbol); });
        assert(syms == std::vector<std::string>({"A", "B", "C"}));
    }

    // 4. RL Rotation (Inserting in Left child of Right subtree: A, C, B)
    {
        TickerTree tree;
        tree.insert("A");
        tree.insert("C");
        tree.insert("B"); // Triggers RL rotation (rotateRight on C then rotateLeft on A)
        assert(tree.height() == 2);
        std::vector<std::string> syms;
        tree.inorder([&syms](TickerNode* n) { syms.push_back(n->symbol); });
        assert(syms == std::vector<std::string>({"A", "B", "C"}));
    }

    std::cout << "[PASS] testRotationsIndividually (LL, RR, LR, RL)\n";
}

void testDeletions() {
    TickerTree tree;
    tree.insert("D");
    tree.insert("B");
    tree.insert("F");
    tree.insert("A");
    tree.insert("C");
    tree.insert("E");
    tree.insert("G");

    assert(tree.size() == 7);

    // 1. Delete leaf node ("A")
    assert(tree.remove("A"));
    assert(tree.size() == 6);
    assert(tree.find("A") == nullptr);

    // 2. Delete node with one child
    // Add child to B
    tree.insert("A"); // B now has left child A and right child C
    tree.remove("C"); // Now B has only left child A
    assert(tree.remove("B")); // Delete node with one child (A)
    assert(tree.find("B") == nullptr);

    // 3. Delete node with two children ("F" which has children E and G)
    assert(tree.remove("F"));
    assert(tree.find("F") == nullptr);

    // 4. Delete Root node ("D")
    assert(tree.remove("D"));
    assert(tree.find("D") == nullptr);

    std::cout << "[PASS] testDeletions (Leaf, One-Child, Two-Children, Root)\n";
}

void testDuplicateInsert() {
    TickerTree tree;
    TickerNode* n1 = tree.insert("RELIANCE");
    assert(tree.size() == 1);

    TickerNode* n2 = tree.insert("RELIANCE");
    assert(tree.size() == 1);
    assert(n1 == n2);

    std::cout << "[PASS] testDuplicateInsert\n";
}

void testSortedInsertBalance() {
    TickerTree tree;
    std::vector<std::string> tickers = {
        "HDFCBANK", "INFY", "ITC", "RELIANCE", "SBIN", "TATAMOTORS", "TCS", "WIPRO"
    };

    for (const auto& sym : tickers) {
        tree.insert(sym);
    }

    assert(tree.size() == 8);
    assert(tree.height() == 4); // Explicit height assertion for 8 sorted inserts

    std::cout << "[PASS] testSortedInsertBalance (Explicit Height = 4 for 8 nodes)\n";
}

void testRandomized1000Ops() {
    TickerTree tree;
    std::mt19937 rng(42); // fixed seed
    std::vector<std::string> activeKeys;

    for (int op = 0; op < 1000; ++op) {
        int choice = rng() % 3;
        int val = rng() % 200;
        std::string key = "TICKER_" + std::to_string(val);

        if (choice == 0 || choice == 1 || activeKeys.empty()) {
            // Insert
            tree.insert(key);
            if (std::find(activeKeys.begin(), activeKeys.end(), key) == activeKeys.end()) {
                activeKeys.push_back(key);
            }
        } else {
            // Delete
            int idx = rng() % activeKeys.size();
            std::string delKey = activeKeys[idx];
            bool res = tree.remove(delKey);
            assert(res);
            activeKeys.erase(activeKeys.begin() + idx);
        }

        // Check height and balance factors
        checkHeightAndBalance(tree.getRoot());

        // Verify size
        assert(tree.size() == activeKeys.size());

        // Verify BST order and AVL balance factor invariant
        std::vector<std::string> inorderSyms;
        tree.inorder([&inorderSyms](TickerNode* n) {
            inorderSyms.push_back(n->symbol);
        });

        // Check strictly ascending
        for (size_t i = 1; i < inorderSyms.size(); ++i) {
            assert(inorderSyms[i - 1] < inorderSyms[i]);
        }
    }

    std::cout << "[PASS] testRandomized1000Ops (1000 random ops, strictly sorted, balance factors invariant)\n";
}

int main() {
    std::cout << "--- Running Expanded TickerTree AVL Unit Tests ---\n";
    testRotationsIndividually();
    testDeletions();
    testDuplicateInsert();
    testSortedInsertBalance();
    testRandomized1000Ops();
    std::cout << "All TickerTree AVL tests passed successfully!\n";
    return 0;
}
