/**
 * test_portfolio.cpp - Unit tests for PortfolioManager logic and math verification.
 */
#include "PortfolioManager.h"
#include <cassert>
#include <iostream>
#include <cmath>

static bool isClose(double a, double b, double tol = 0.001) {
    return std::fabs(a - b) < tol;
}

void testWeightedAverageAndHoldings() {
    PortfolioManager manager;

    // Broker A buys 10 RELIANCE @ 2400 (Total cost = 24,000)
    bool b1 = manager.applyTrade(Trade("Groww", "RELIANCE", TradeSide::BUY, 10, 2400.0, "2023-01-10"));
    assert(b1);

    // Broker A buys 10 RELIANCE @ 2500 (Total cost = 25,000)
    bool b2 = manager.applyTrade(Trade("Groww", "RELIANCE", TradeSide::BUY, 10, 2500.0, "2023-01-20"));
    assert(b2);

    TickerNode* node = manager.getTickers().find("RELIANCE");
    assert(node != nullptr);
    Holding* hGroww = node->findHolding("Groww");
    assert(hGroww != nullptr);

    assert(hGroww->getQty() == 20);
    assert(isClose(hGroww->getAvgCost(), 2450.00));
    assert(isClose(hGroww->investedAmount(), 49000.00));

    // Broker B buys 10 RELIANCE @ 2600 (Total cost = 26,000)
    manager.applyTrade(Trade("Zerodha", "RELIANCE", TradeSide::BUY, 10, 2600.0, "2023-02-01"));

    assert(node->totalQty() == 30);
    assert(isClose(node->totalInvested(), 75000.00));
    assert(isClose(node->blendedAvgCost(), 2500.00));

    std::cout << "[PASS] testWeightedAverageAndHoldings\n";
}

void testSellAndRealizedPL() {
    PortfolioManager manager;

    // Groww buys 20 INFY @ 1400.0 (Cost = 28,000)
    manager.applyTrade(Trade("Groww", "INFY", TradeSide::BUY, 20, 1400.0, "2023-01-05"));

    // Groww sells 5 INFY @ 1500.0
    // Expected Realized P/L = 5 * (1500 - 1400) = +500.00
    bool bSell = manager.applyTrade(Trade("Groww", "INFY", TradeSide::SELL, 5, 1500.0, "2023-01-25"));
    assert(bSell);

    TickerNode* node = manager.getTickers().find("INFY");
    Holding* hGroww = node->findHolding("Groww");
    assert(hGroww->getQty() == 15);
    assert(isClose(hGroww->getAvgCost(), 1400.00)); // Avg cost unchanged
    assert(isClose(hGroww->getRealizedPL(), 500.00));

    std::cout << "[PASS] testSellAndRealizedPL\n";
}

void testOversellRejection() {
    PortfolioManager manager;
    manager.applyTrade(Trade("Zerodha", "TCS", TradeSide::BUY, 10, 3200.0, "2023-01-10"));

    // Attempt to sell 15 TCS when holding only 10
    bool bOversell = manager.applyTrade(Trade("Zerodha", "TCS", TradeSide::SELL, 15, 3300.0, "2023-01-20"));
    assert(!bOversell); // Must be rejected!

    TickerNode* node = manager.getTickers().find("TCS");
    Holding* hZerodha = node->findHolding("Zerodha");
    assert(hZerodha->getQty() == 10); // Qty remains 10

    std::cout << "[PASS] testOversellRejection\n";
}

void testPortfolioTotalsAndUnrealizedPL() {
    PortfolioManager manager;
    manager.applyTrade(Trade("Groww", "HDFCBANK", TradeSide::BUY, 20, 1500.0, "2023-01-01")); // 30,000
    manager.applyTrade(Trade("Zerodha", "HDFCBANK", TradeSide::BUY, 10, 1530.0, "2023-01-05")); // 15,300

    assert(isClose(manager.getTotalInvested(), 45300.00));

    // Update LTP to 1600.00
    manager.applyPriceUpdate("HDFCBANK", 1600.0, "10:00:00");
    // Current value = 30 * 1600 = 48,000
    // Unrealized PL = 48000 - 45300 = +2700.00
    assert(isClose(manager.getCurrentValue(), 48000.00));
    assert(isClose(manager.getUnrealizedPL(), 2700.00));

    std::cout << "[PASS] testPortfolioTotalsAndUnrealizedPL\n";
}

int main() {
    std::cout << "--- Running PortfolioManager Unit Tests ---\n";
    testWeightedAverageAndHoldings();
    testSellAndRealizedPL();
    testOversellRejection();
    testPortfolioTotalsAndUnrealizedPL();
    std::cout << "All PortfolioManager tests passed successfully!\n";
    return 0;
}
