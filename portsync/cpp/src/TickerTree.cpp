/**
 * TickerTree.cpp - Implementation of self-balancing AVL Binary Search Tree.
 * Includes LL, RR, LR, RL tree rotations to maintain O(log n) performance.
 */
#include "TickerTree.h"
#include <algorithm>
#include <utility>

TickerTree::TickerTree() : m_root(nullptr), m_size(0) {}

TickerTree::~TickerTree() {
    clear();
}

TickerTree::TickerTree(TickerTree&& other) noexcept
    : m_root(other.m_root), m_size(other.m_size) {
    other.m_root = nullptr;
    other.m_size = 0;
}

TickerTree& TickerTree::operator=(TickerTree&& other) noexcept {
    if (this != &other) {
        clear();
        m_root = other.m_root;
        m_size = other.m_size;
        other.m_root = nullptr;
        other.m_size = 0;
    }
    return *this;
}

void TickerTree::clear() {
    freeTree(m_root);
    m_root = nullptr;
    m_size = 0;
}

void TickerTree::freeTree(TickerNode* node) {
    if (node == nullptr) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

int TickerTree::getHeight(TickerNode* node) const {
    return node ? node->height : 0;
}

int TickerTree::getBalanceFactor(TickerNode* node) const {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

void TickerTree::updateHeight(TickerNode* node) {
    if (node) {
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }
}

TickerNode* TickerTree::rotateRight(TickerNode* y) {
    TickerNode* x = y->left;
    TickerNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

TickerNode* TickerTree::rotateLeft(TickerNode* x) {
    TickerNode* y = x->right;
    TickerNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

TickerNode* TickerTree::insertNode(TickerNode* node, const std::string& symbol, TickerNode** insertedOrFoundNode) {
    if (node == nullptr) {
        TickerNode* newNode = new TickerNode(symbol);
        *insertedOrFoundNode = newNode;
        m_size++;
        return newNode;
    }

    if (symbol < node->symbol) {
        node->left = insertNode(node->left, symbol, insertedOrFoundNode);
    } else if (symbol > node->symbol) {
        node->right = insertNode(node->right, symbol, insertedOrFoundNode);
    } else {
        // Symbol already exists
        *insertedOrFoundNode = node;
        return node;
    }

    updateHeight(node);
    int bf = getBalanceFactor(node);

    // Left Left Case
    if (bf > 1 && symbol < node->left->symbol) {
        return rotateRight(node);
    }

    // Right Right Case
    if (bf < -1 && symbol > node->right->symbol) {
        return rotateLeft(node);
    }

    // Left Right Case
    if (bf > 1 && symbol > node->left->symbol) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (bf < -1 && symbol < node->right->symbol) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

TickerNode* TickerTree::insert(const std::string& symbol) {
    TickerNode* targetNode = nullptr;
    m_root = insertNode(m_root, symbol, &targetNode);
    return targetNode;
}

TickerNode* TickerTree::findNode(TickerNode* node, const std::string& symbol) const {
    if (node == nullptr || node->symbol == symbol) {
        return node;
    }
    if (symbol < node->symbol) {
        return findNode(node->left, symbol);
    }
    return findNode(node->right, symbol);
}

TickerNode* TickerTree::find(const std::string& symbol) const {
    return findNode(m_root, symbol);
}

TickerNode* TickerTree::findMin(TickerNode* node) const {
    TickerNode* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

TickerNode* TickerTree::removeNode(TickerNode* node, const std::string& symbol) {
    if (node == nullptr) return nullptr;

    if (symbol < node->symbol) {
        node->left = removeNode(node->left, symbol);
    } else if (symbol > node->symbol) {
        node->right = removeNode(node->right, symbol);
    } else {
        // Found node to delete
        if (node->left == nullptr || node->right == nullptr) {
            TickerNode* temp = node->left ? node->left : node->right;
            if (temp == nullptr) {
                // No child
                temp = node;
                node = nullptr;
            } else {
                // One child
                *node = *temp; // Copy contents
            }
            delete temp;
            m_size--;
        } else {
            // Node with two children
            TickerNode* temp = findMin(node->right);
            node->symbol = temp->symbol;
            node->holdings = temp->holdings;
            node->latestPrice = temp->latestPrice;
            node->priceTime = temp->priceTime;
            node->newsList = temp->newsList;
            node->history = temp->history;

            node->right = removeNode(node->right, temp->symbol);
        }
    }

    if (node == nullptr) return nullptr;

    updateHeight(node);
    int bf = getBalanceFactor(node);

    // Left Left
    if (bf > 1 && getBalanceFactor(node->left) >= 0) {
        return rotateRight(node);
    }

    // Left Right
    if (bf > 1 && getBalanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Right
    if (bf < -1 && getBalanceFactor(node->right) <= 0) {
        return rotateLeft(node);
    }

    // Right Left
    if (bf < -1 && getBalanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

bool TickerTree::remove(const std::string& symbol) {
    if (!find(symbol)) return false;
    m_root = removeNode(m_root, symbol);
    return true;
}

void TickerTree::inorderHelper(TickerNode* node, const std::function<void(TickerNode*)>& callback) const {
    if (node == nullptr) return;
    inorderHelper(node->left, callback);
    callback(node);
    inorderHelper(node->right, callback);
}

void TickerTree::inorder(const std::function<void(TickerNode*)>& callback) const {
    inorderHelper(m_root, callback);
}
