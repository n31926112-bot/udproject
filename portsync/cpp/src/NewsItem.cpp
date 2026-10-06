/**
 * NewsItem.cpp - Implementation of NewsItem class.
 */
#include "NewsItem.h"

NewsItem::NewsItem(std::string ticker, std::string date, std::string headline)
    : m_ticker(std::move(ticker)), m_date(std::move(date)), m_headline(std::move(headline)) {}

std::string NewsItem::toString() const {
    return "[" + m_date + "] " + m_ticker + ": " + m_headline;
}
