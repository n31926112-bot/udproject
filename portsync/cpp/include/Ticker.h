/**
 * Ticker.h - Node data structure for AVL tree representing a stock ticker.
 * Holds per-broker holding breakdown, latest price, news list, and history.
 */
#ifndef TICKER_H
#define TICKER_H

#include "Holding.h"
#include "NewsItem.h"
#include <string>
#include <vector>
#include <utility>

struct HistClose {
    std::string date;
    double close;

    HistClose(std::string d = "", double c = 0.0) : date(std::move(d)), close(c) {}
};

struct TickerNode {
    std::string symbol;
    std::vector<Holding> holdings; // per-broker holdings for this ticker
    double latestPrice;
    std::string priceTime;
    std::vector<NewsItem> newsList;
    std::vector<HistClose> history;

    // AVL Tree pointers and height
    TickerNode* left;
    TickerNode* right;
    int height;

    explicit TickerNode(std::string sym)
        : symbol(std::move(sym)),
          latestPrice(0.0),
          priceTime(""),
          left(nullptr),
          right(nullptr),
          height(1) {}

    // Find holding by broker name, or nullptr if not found
    Holding* findHolding(const std::string& broker) {
        for (auto& h : holdings) {
            if (h.getBroker() == broker) return &h;
        }
        return nullptr;
    }

    const Holding* findHolding(const std::string& broker) const {
        for (const auto& h : holdings) {
            if (h.getBroker() == broker) return &h;
        }
        return nullptr;
    }

    Holding& getOrCreateHolding(const std::string& broker) {
        Holding* existing = findHolding(broker);
        if (existing) return *existing;
        holdings.emplace_back(broker, symbol);
        return holdings.back();
    }

    // Aggregated statistics across all brokers for this ticker
    int totalQty() const {
        int total = 0;
        for (const auto& h : holdings) total += h.getQty();
        return total;
    }

    double totalInvested() const {
        double sum = 0.0;
        for (const auto& h : holdings) sum += h.investedAmount();
        return sum;
    }

    double blendedAvgCost() const {
        int q = totalQty();
        if (q == 0) return 0.0;
        return totalInvested() / q;
    }

    double currentValue() const {
        return totalQty() * latestPrice;
    }

    double unrealizedPL() const {
        return currentValue() - totalInvested();
    }

    double unrealizedPLPercent() const {
        double inv = totalInvested();
        if (inv == 0.0) return 0.0;
        return (unrealizedPL() / inv) * 100.0;
    }

    double totalRealizedPL() const {
        double sum = 0.0;
        for (const auto& h : holdings) sum += h.getRealizedPL();
        return sum;
    }
};

#endif // TICKER_H
