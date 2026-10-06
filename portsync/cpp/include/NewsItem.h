#ifndef NEWS_ITEM_H
#define NEWS_ITEM_H

#include "Printable.h"
#include <string>

class NewsItem : public Printable {
private:
    std::string m_ticker;
    std::string m_date;
    std::string m_headline;

public:
    NewsItem(std::string ticker = "", std::string date = "", std::string headline = "");

    const std::string& getTicker() const { return m_ticker; }
    const std::string& getDate() const { return m_date; }
    const std::string& getHeadline() const { return m_headline; }

    std::string toString() const override;
};

#endif
