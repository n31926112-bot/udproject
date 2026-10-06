#ifndef HOLDING_H
#define HOLDING_H

#include "Printable.h"
#include <string>

class Holding : public Printable {
private:
    std::string m_broker;
    std::string m_ticker;
    int m_qty;
    double m_avgCost;
    double m_realizedPL;

public:
    Holding(std::string broker = "", std::string ticker = "");

    const std::string& getBroker() const { return m_broker; }
    const std::string& getTicker() const { return m_ticker; }
    int getQty() const { return m_qty; }
    double getAvgCost() const { return m_avgCost; }
    double getRealizedPL() const { return m_realizedPL; }
    double investedAmount() const { return m_qty * m_avgCost; }

    void applyBuy(int qty, double price);
    bool applySell(int qty, double price);

    std::string toString() const override;
};

#endif
