/**
 * Dashboard.cpp - Implementation of CLI portfolio rendering and polymorphic stream output.
 */
#include "Dashboard.h"
#include "Projection.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>

Dashboard::Dashboard(bool useColor) : m_useColor(useColor) {}

void Dashboard::renderPolymorphicSummary(const std::vector<const Printable*>& items) const {
    std::cout << colorBold() << colorCyan() << "\n=== POLYMORPHIC VIRTUAL DISPATCH STREAM (Printable*) ===" << colorReset() << "\n";
    for (const Printable* item : items) {
        if (item) {
            std::cout << "  [Printable -> " << item->toString() << "]\n";
        }
    }
    std::cout << "========================================================\n\n";
}

void Dashboard::render(const PortfolioManager& manager, int batchSeq) const {
    // Clear screen if TTY
    if (m_useColor) {
        std::cout << "\033[2J\033[1;1H";
    }

    std::time_t now = std::time(nullptr);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::cout << colorBold() << colorCyan()
              << "========================================================================================\n"
              << "                         PORTSYNC CROSS-BROKER PORTFOLIO DASHBOARD                       \n"
              << "                           Timestamp: " << timeStr << " | Batch #" << batchSeq << "\n"
              << "========================================================================================\n"
              << colorReset();

    // 1. Unified Portfolio Table
    std::cout << colorBold() << "\n[1] UNIFIED PORTFOLIO HOLDINGS (Merged Across Brokers)" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(12) << "TICKER"
              << std::right << std::setw(8) << "QTY"
              << std::setw(12) << "AVG COST"
              << std::setw(12) << "LTP"
              << std::setw(14) << "INVESTED"
              << std::setw(14) << "CURR VALUE"
              << std::setw(12) << "P/L"
              << std::setw(10) << "P/L %" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    std::vector<const Printable*> polymorphicItems;

    manager.getTickers().inorder([this, &polymorphicItems](TickerNode* node) {
        if (node->totalQty() == 0) return;

        double pl = node->unrealizedPL();
        double plPct = node->unrealizedPLPercent();
        std::string pColor = (pl >= 0) ? colorGreen() : colorRed();

        std::cout << std::left << std::setw(12) << node->symbol
                  << std::right << std::setw(8) << node->totalQty()
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << node->blendedAvgCost()
                  << std::setw(12) << node->latestPrice
                  << std::setw(14) << node->totalInvested()
                  << std::setw(14) << node->currentValue()
                  << pColor << std::setw(12) << pl
                  << std::setw(9) << plPct << "%" << colorReset() << "\n";

        // Collect holdings for polymorphic virtual dispatch demonstration
        for (const auto& h : node->holdings) {
            polymorphicItems.push_back(&h);
        }
    });

    std::cout << "----------------------------------------------------------------------------------------\n";

    // Totals Row
    double totalInv = manager.getTotalInvested();
    double currVal = manager.getCurrentValue();
    double unrealizedPL = manager.getUnrealizedPL();
    double realizedPL = manager.getRealizedPL();
    double totalPLPct = (totalInv > 0) ? (unrealizedPL / totalInv) * 100.0 : 0.0;
    std::string totColor = (unrealizedPL >= 0) ? colorGreen() : colorRed();

    std::cout << std::left << std::setw(12) << "TOTALS"
              << std::right << std::setw(8) << "-"
              << std::setw(12) << "-"
              << std::setw(12) << "-"
              << std::fixed << std::setprecision(2)
              << std::setw(14) << totalInv
              << std::setw(14) << currVal
              << totColor << std::setw(12) << unrealizedPL
              << std::setw(9) << totalPLPct << "%" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "  * Realized P/L from Closed Positions: "
              << ((realizedPL >= 0) ? colorGreen() : colorRed())
              << std::fixed << std::setprecision(2) << realizedPL << colorReset() << "\n\n";

    // 2. Broker Breakdown & Comparison
    std::cout << colorBold() << "[2] BROKER COMPARISON SUMMARY" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(15) << "BROKER"
              << std::right << std::setw(16) << "INVESTED"
              << std::setw(16) << "CURR VALUE"
              << std::setw(16) << "UNREALIZED P/L"
              << std::setw(16) << "REALIZED P/L" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    auto summaries = manager.getBrokerSummaries();
    for (const auto& s : summaries) {
        std::string sColor = (s.unrealizedPL >= 0) ? colorGreen() : colorRed();
        std::cout << std::left << std::setw(15) << s.broker
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(16) << s.investedAmount
                  << std::setw(16) << s.currentValue
                  << sColor << std::setw(16) << s.unrealizedPL << colorReset()
                  << std::setw(16) << s.realizedPL << "\n";
    }
    std::cout << "----------------------------------------------------------------------------------------\n\n";

    // 3. One-Year Estimated Projections (Honest Labelling)
    std::cout << colorBold() << "[3] 1-YEAR ESTIMATED PROJECTIONS (Disclaimer: ESTIMATE ONLY - NOT A PREDICTION)" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(12) << "TICKER"
              << std::right << std::setw(12) << "LTP"
              << std::setw(16) << "EST 1-YR PRICE"
              << std::setw(14) << "EST RETURN %"
              << "   METHOD USED" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    manager.getTickers().inorder([this](TickerNode* node) {
        if (node->totalQty() == 0) return;
        ProjectionResult proj = Projection::calculateOneYearEstimate(*node);
        std::string rColor = (proj.expectedReturnPct >= 0) ? colorGreen() : colorRed();

        std::cout << std::left << std::setw(12) << node->symbol
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(12) << proj.currentPrice
                  << std::setw(16) << proj.estimatedPrice1Yr
                  << rColor << std::setw(13) << proj.expectedReturnPct << "%" << colorReset()
                  << "   " << proj.method << "\n";
    });
    std::cout << "----------------------------------------------------------------------------------------\n\n";

    // 4. Latest News Headlines (Max 2 per ticker)
    std::cout << colorBold() << "[4] LATEST MARKET NEWS HEADLINES" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    manager.getTickers().inorder([this, &polymorphicItems](TickerNode* node) {
        if (node->totalQty() == 0 || node->newsList.empty()) return;
        size_t count = 0;
        for (const auto& news : node->newsList) {
            if (count++ >= 2) break;
            std::cout << "  - [" << news.getDate() << "] " << colorYellow() << news.getTicker() << colorReset()
                      << ": " << news.getHeadline() << "\n";
            polymorphicItems.push_back(&news);
        }
    });
    std::cout << "----------------------------------------------------------------------------------------\n\n";

    // 5. Recent Trade History (Last 5 trades via virtual dispatch sample)
    std::cout << colorBold() << "[5] RECENT TRADE HISTORY LOG (Last 5 Trades)" << colorReset() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    int tradeCount = 0;
    manager.getTrades().forEach([&tradeCount, &polymorphicItems](const Trade& t) {
        if (tradeCount++ < 5) {
            std::cout << "  " << t.toString() << "\n";
            polymorphicItems.push_back(&t);
        }
    });
    std::cout << "----------------------------------------------------------------------------------------\n\n";

    // Render Polymorphic Virtual Dispatch Stream
    renderPolymorphicSummary(polymorphicItems);
}

void Dashboard::interactivePrompt(const PortfolioManager& manager) const {
    std::cout << colorBold() << colorYellow()
              << "Interactive CLI Mode active. Commands: 'search <TICKER>', 'trades', 'quit'\n"
              << colorReset();

    std::string line;
    while (true) {
        std::cout << "PortSync> ";
        if (!std::getline(std::cin, line) || line == "quit") {
            break;
        }
        if (line.substr(0, 7) == "search ") {
            std::string sym = line.substr(7);
            // Transform to uppercase
            for (auto& c : sym) c = std::toupper(c);

            TickerNode* node = manager.getTickers().find(sym);
            if (node) {
                std::cout << "\nFound Ticker in AVL Tree: " << node->symbol << "\n"
                          << "  - Total Qty: " << node->totalQty() << "\n"
                          << "  - Blended Avg Cost: " << node->blendedAvgCost() << "\n"
                          << "  - Latest Price: " << node->latestPrice << "\n"
                          << "  - Current Value: " << node->currentValue() << "\n"
                          << "  - Unrealized P/L: " << node->unrealizedPL() << "\n";
                for (const auto& h : node->holdings) {
                    std::cout << "  - Holding via " << h.getBroker() << ": Qty " << h.getQty() << " @ Avg " << h.getAvgCost() << "\n";
                }
                std::cout << "\n";
            } else {
                std::cout << "Ticker '" << sym << "' not found in AVL tree.\n\n";
            }
        } else if (line == "trades") {
            std::cout << "\nAll Historical Trades:\n";
            manager.getTrades().forEach([](const Trade& t) {
                std::cout << "  " << t.toString() << "\n";
            });
            std::cout << "\n";
        }
    }
}
