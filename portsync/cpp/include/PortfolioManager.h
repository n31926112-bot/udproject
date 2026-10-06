/**
 * PortfolioManager.h - Central manager maintaining portfolio trades and ticker tree.
 * Demonstrates: Object composition, business domain aggregation, const correctness.
 */
#ifndef PORTFOLIO_MANAGER_H
#define PORTFOLIO_MANAGER_H

#include "TradeList.h"
#include "TickerTree.h"
#include "NewsItem.h"
#include <string>
#include <vector>
#include <map>

struct BrokerSummary {
    std::string broker;
    double investedAmount;
    double currentValue;
    double unrealizedPL;
    double realizedPL;

    BrokerSummary(std::string b = "")
        : broker(std::move(b)), investedAmount(0.0), currentValue(0.0), unrealizedPL(0.0), realizedPL(0.0) {}
};

class PortfolioManager {
private:
    TradeList m_trades;
    TickerTree m_tickers;

public:
    PortfolioManager() = default;
    ~PortfolioManager() = default;

    // Rule of 5: Disable copies
    PortfolioManager(const PortfolioManager&) = delete;
    PortfolioManager& operator=(const PortfolioManager&) = delete;

    bool applyTrade(const Trade& trade);
    void applyPriceUpdate(const std::string& ticker, double price, const std::string& timeStr);
    void addNews(const NewsItem& news);
    void addHistClose(const std::string& ticker, const std::string& date, double close);

    const TradeList& getTrades() const { return m_trades; }
    const TickerTree& getTickers() const { return m_tickers; }

    double getTotalInvested() const;
    double getCurrentValue() const;
    double getUnrealizedPL() const;
    double getRealizedPL() const;

    std::vector<BrokerSummary> getBrokerSummaries() const;
};

#endif // PORTFOLIO_MANAGER_H
