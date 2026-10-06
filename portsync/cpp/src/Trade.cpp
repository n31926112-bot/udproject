#include "Trade.h"
#include <sstream>
#include <iomanip>

Trade::Trade(std::string broker, std::string ticker, TradeSide side, int qty, double price, std::string date)
    : m_broker(std::move(broker)),
      m_ticker(std::move(ticker)),
      m_side(side),
      m_qty(qty),
      m_price(price),
      m_date(std::move(date)) {}

std::string Trade::toString() const {
    std::ostringstream oss;
    oss << "[" << m_date << "] " << (m_side == TradeSide::BUY ? "BUY " : "SELL ")
        << m_qty << " @ " << std::fixed << std::setprecision(2) << m_price
        << " (" << m_ticker << " via " << m_broker << ")";
    return oss.str();
}
