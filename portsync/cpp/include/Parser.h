/**
 * Parser.h - Line parser using std::stringstream for wire protocol records.
 * Demonstrates: Robust string parsing, stringstream utilization, exception safety.
 */
#ifndef PARSER_H
#define PARSER_H

#include "Trade.h"
#include "NewsItem.h"
#include <string>
#include <vector>

enum class ParseResultType {
    HELLO,
    TRADE,
    PRICE,
    HIST,
    NEWS,
    BATCH_END,
    BYE,
    MALFORMED,
    UNKNOWN
};

struct ParseResult {
    ParseResultType type;
    std::string broker;
    std::string ticker;
    TradeSide side;
    int qty;
    double price;
    std::string dateOrTime;
    std::string headline;
    int seq;
    int version;
    std::string errorMsg;

    ParseResult()
        : type(ParseResultType::UNKNOWN),
          side(TradeSide::BUY),
          qty(0),
          price(0.0),
          seq(0),
          version(0) {}
};

class Parser {
public:
    static ParseResult parseLine(const std::string& line, int lineNumber);

private:
    static std::vector<std::string> splitPipe(const std::string& line);
};

#endif // PARSER_H
