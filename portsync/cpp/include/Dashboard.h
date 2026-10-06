/**
 * Dashboard.h - Terminal CLI UI renderer for PortSync portfolio dashboard.
 * Demonstrates: Polymorphic rendering (virtual dispatch over Printable base pointers),
 * tabular CLI formatting with std::iomanip / printf, ANSI color controls.
 */
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "PortfolioManager.h"
#include "Printable.h"
#include <string>
#include <vector>

class Dashboard {
private:
    bool m_useColor;

    // ANSI Color Codes
    std::string colorReset() const { return m_useColor ? "\033[0m" : ""; }
    std::string colorBold() const { return m_useColor ? "\033[1m" : ""; }
    std::string colorGreen() const { return m_useColor ? "\033[32m" : ""; }
    std::string colorRed() const { return m_useColor ? "\033[31m" : ""; }
    std::string colorYellow() const { return m_useColor ? "\033[33m" : ""; }
    std::string colorCyan() const { return m_useColor ? "\033[36m" : ""; }

public:
    explicit Dashboard(bool useColor = true);

    void setUseColor(bool useColor) { m_useColor = useColor; }

    // Renders complete dashboard sections
    void render(const PortfolioManager& manager, int batchSeq) const;

    // Demonstrates virtual dispatch over abstract Printable base pointers
    void renderPolymorphicSummary(const std::vector<const Printable*>& items) const;

    // Interactive CLI prompt mode (search <TICKER>, trades, quit)
    void interactivePrompt(const PortfolioManager& manager) const;
};

#endif // DASHBOARD_H
