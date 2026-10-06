#include "Holding.h"
#include <sstream>
#include <iomanip>

Holding::Holding(std::string broker, std::string ticker)
    : m_broker(std::move(broker)),
      m_ticker(std::move(ticker)),
      m_qty(0),
      m_avgCost(0.0),
      m_realizedPL(0.0) {}

void Holding::applyBuy(int qty, double price) {
    if (qty <= 0) return;
    // Weighted average: newAvg = (oldQty*oldAvg + newQty*newPrice) / totalQty
    double currentTotalCost = m_qty * m_avgCost;
    double newBuyCost = qty * price;
    m_qty += qty;
    m_avgCost = (currentTotalCost + newBuyCost) / m_qty;
}

bool Holding::applySell(int qty, double price) {
    if (qty <= 0 || qty > m_qty) return false;
    // Realized P/L = sold_qty * (sell_price - avg_cost)
    m_realizedPL += qty * (price - m_avgCost);
    m_qty -= qty;
    if (m_qty == 0) m_avgCost = 0.0;
    return true;
}

std::string Holding::toString() const {
    std::ostringstream oss;
    oss << m_broker << " | " << m_ticker << " | Qty: " << m_qty
        << " | Avg: " << std::fixed << std::setprecision(2) << m_avgCost
        << " | Realized P/L: " << m_realizedPL;
    return oss.str();
}
