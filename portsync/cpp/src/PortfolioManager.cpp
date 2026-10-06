/**
 * PortfolioManager.cpp - Implementation of portfolio tracking & aggregation.
 */
#include "PortfolioManager.h"
#include <iostream>
#include <map>

bool PortfolioManager::applyTrade(const Trade& trade) {
    TickerNode* node = m_tickers.insert(trade.getTicker());
    Holding& holding = node->getOrCreateHolding(trade.getBroker());

    if (trade.getSide() == TradeSide::BUY) {
        holding.applyBuy(trade.getQty(), trade.getPrice());
        m_trades.insertSorted(trade);
        return true;
    } else if (trade.getSide() == TradeSide::SELL) {
        if (!holding.applySell(trade.getQty(), trade.getPrice())) {
            std::cerr << "[REJECTED SELL] " << trade.getBroker() << " oversell attempt on "
                      << trade.getTicker() << ": trying to sell " << trade.getQty()
                      << " but only holding " << holding.getQty() << "\n";
            return false;
        }
        m_trades.insertSorted(trade);
        return true;
    }
    return false;
}

void PortfolioManager::applyPriceUpdate(const std::string& ticker, double price, const std::string& timeStr) {
    TickerNode* node = m_tickers.insert(ticker);
    node->latestPrice = price;
    node->priceTime = timeStr;
}

void PortfolioManager::addNews(const NewsItem& news) {
    TickerNode* node = m_tickers.insert(news.getTicker());
    node->newsList.push_back(news);
}

void PortfolioManager::addHistClose(const std::string& ticker, const std::string& date, double close) {
    TickerNode* node = m_tickers.insert(ticker);
    node->history.emplace_back(date, close);
}

double PortfolioManager::getTotalInvested() const {
    double total = 0.0;
    m_tickers.inorder([&total](TickerNode* node) {
        total += node->totalInvested();
    });
    return total;
}

double PortfolioManager::getCurrentValue() const {
    double total = 0.0;
    m_tickers.inorder([&total](TickerNode* node) {
        total += node->currentValue();
    });
    return total;
}

double PortfolioManager::getUnrealizedPL() const {
    return getCurrentValue() - getTotalInvested();
}

double PortfolioManager::getRealizedPL() const {
    double total = 0.0;
    m_tickers.inorder([&total](TickerNode* node) {
        total += node->totalRealizedPL();
    });
    return total;
}

std::vector<BrokerSummary> PortfolioManager::getBrokerSummaries() const {
    std::map<std::string, BrokerSummary> summaryMap;

    m_tickers.inorder([&summaryMap](TickerNode* node) {
        for (const auto& h : node->holdings) {
            auto& summary = summaryMap[h.getBroker()];
            summary.broker = h.getBroker();
            summary.investedAmount += h.investedAmount();
            summary.currentValue += h.getQty() * node->latestPrice;
            summary.unrealizedPL += (h.getQty() * node->latestPrice) - h.investedAmount();
            summary.realizedPL += h.getRealizedPL();
        }
    });

    std::vector<BrokerSummary> result;
    for (const auto& kv : summaryMap) {
        result.push_back(kv.second);
    }
    return result;
}
