#ifndef TRADE_H
#define TRADE_H

#include "Printable.h"
#include <string>

enum class TradeSide { BUY, SELL };

class Trade : public Printable {
private:
    std::string m_broker;
    std::string m_ticker;
    TradeSide m_side;
    int m_qty;
    double m_price;
    std::string m_date;

public:
    Trade(std::string broker, std::string ticker, TradeSide side, int qty, double price, std::string date);

    const std::string& getBroker() const { return m_broker; }
    const std::string& getTicker() const { return m_ticker; }
    TradeSide getSide() const { return m_side; }
    int getQty() const { return m_qty; }
    double getPrice() const { return m_price; }
    const std::string& getDate() const { return m_date; }

    double amount() const { return m_qty * m_price; }
    std::string toString() const override;
};

#endif
