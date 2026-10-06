/**
 * test_parser.cpp - Unit tests for Parser class.
 */
#include "Parser.h"
#include <cassert>
#include <iostream>

void testValidParsing() {
    // HELLO
    ParseResult r1 = Parser::parseLine("HELLO|PortSync|1\n", 1);
    assert(r1.type == ParseResultType::HELLO);
    assert(r1.broker == "PortSync");
    assert(r1.version == 1);

    // TRADE
    ParseResult r2 = Parser::parseLine("TRADE|Groww|RELIANCE|BUY|10|2450.50|2023-01-15\n", 2);
    assert(r2.type == ParseResultType::TRADE);
    assert(r2.broker == "Groww");
    assert(r2.ticker == "RELIANCE");
    assert(r2.side == TradeSide::BUY);
    assert(r2.qty == 10);
    assert(r2.price == 2450.50);
    assert(r2.dateOrTime == "2023-01-15");

    // PRICE
    ParseResult r3 = Parser::parseLine("PRICE|TCS|3300.00|10:00:00\n", 3);
    assert(r3.type == ParseResultType::PRICE);
    assert(r3.ticker == "TCS");
    assert(r3.price == 3300.00);
    assert(r3.dateOrTime == "10:00:00");

    // HIST
    ParseResult r4 = Parser::parseLine("HIST|INFY|2023-01-02|1400.00\n", 4);
    assert(r4.type == ParseResultType::HIST);
    assert(r4.ticker == "INFY");
    assert(r4.dateOrTime == "2023-01-02");
    assert(r4.price == 1400.00);

    // NEWS
    ParseResult r5 = Parser::parseLine("NEWS|SBIN|2023-09-15|SBI raises infrastructure bonds\n", 5);
    assert(r5.type == ParseResultType::NEWS);
    assert(r5.ticker == "SBIN");
    assert(r5.headline == "SBI raises infrastructure bonds");

    // BATCH_END
    ParseResult r6 = Parser::parseLine("BATCH_END|1\n", 6);
    assert(r6.type == ParseResultType::BATCH_END);
    assert(r6.seq == 1);

    // BYE
    ParseResult r7 = Parser::parseLine("BYE\n", 7);
    assert(r7.type == ParseResultType::BYE);

    std::cout << "[PASS] testValidParsing\n";
}

void testMalformedParsing() {
    // Missing tokens
    ParseResult r1 = Parser::parseLine("TRADE|Groww|RELIANCE|BUY|10\n", 1);
    assert(r1.type == ParseResultType::MALFORMED);

    // Invalid trade side
    ParseResult r2 = Parser::parseLine("TRADE|Groww|RELIANCE|HOLD|10|2450.50|2023-01-15\n", 2);
    assert(r2.type == ParseResultType::MALFORMED);

    // Negative quantity
    ParseResult r3 = Parser::parseLine("TRADE|Groww|RELIANCE|BUY|-10|2450.50|2023-01-15\n", 3);
    assert(r3.type == ParseResultType::MALFORMED);

    // Non-numeric price
    ParseResult r4 = Parser::parseLine("PRICE|TCS|abc|10:00:00\n", 4);
    assert(r4.type == ParseResultType::MALFORMED);

    std::cout << "[PASS] testMalformedParsing\n";
}

int main() {
    std::cout << "--- Running Parser Unit Tests ---\n";
    testValidParsing();
    testMalformedParsing();
    std::cout << "All Parser tests passed successfully!\n";
    return 0;
}
