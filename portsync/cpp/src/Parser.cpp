/**
 * Parser.cpp - Implementation of wire protocol stringstream parser.
 */
#include "Parser.h"
#include <sstream>
#include <iostream>

std::vector<std::string> Parser::splitPipe(const std::string& line) {
    std::vector<std::string> tokens;
    std::string cleanLine = line;
    // Strip trailing \r or \n
    while (!cleanLine.empty() && (cleanLine.back() == '\r' || cleanLine.back() == '\n')) {
        cleanLine.pop_back();
    }
    std::stringstream ss(cleanLine);
    std::string token;
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    return tokens;
}

ParseResult Parser::parseLine(const std::string& line, int lineNumber) {
    ParseResult result;
    if (line.empty()) {
        result.type = ParseResultType::MALFORMED;
        result.errorMsg = "Empty line";
        return result;
    }

    std::vector<std::string> tokens = splitPipe(line);
    if (tokens.empty()) {
        result.type = ParseResultType::MALFORMED;
        result.errorMsg = "No tokens found";
        return result;
    }

    const std::string& rtype = tokens[0];

    try {
        if (rtype == "HELLO") {
            if (tokens.size() != 3) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "HELLO requires 3 tokens, got " + std::to_string(tokens.size());
                return result;
            }
            result.type = ParseResultType::HELLO;
            result.broker = tokens[1]; // App Name
            result.version = std::stoi(tokens[2]);
            return result;
        }

        if (rtype == "TRADE") {
            // TRADE|<broker>|<ticker>|<BUY|SELL>|<qty:int>|<price:double>|<YYYY-MM-DD>
            if (tokens.size() != 7) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "TRADE requires 7 tokens, got " + std::to_string(tokens.size());
                return result;
            }
            result.type = ParseResultType::TRADE;
            result.broker = tokens[1];
            result.ticker = tokens[2];
            if (tokens[3] == "BUY") {
                result.side = TradeSide::BUY;
            } else if (tokens[3] == "SELL") {
                result.side = TradeSide::SELL;
            } else {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "Invalid trade side: " + tokens[3];
                return result;
            }
            result.qty = std::stoi(tokens[4]);
            result.price = std::stod(tokens[5]);
            result.dateOrTime = tokens[6];

            if (result.qty <= 0 || result.price <= 0.0) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "Quantity and price must be positive";
                return result;
            }
            return result;
        }

        if (rtype == "PRICE") {
            // PRICE|<ticker>|<price:double>|<HH:MM:SS>
            if (tokens.size() != 4) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "PRICE requires 4 tokens, got " + std::to_string(tokens.size());
                return result;
            }
            result.type = ParseResultType::PRICE;
            result.ticker = tokens[1];
            result.price = std::stod(tokens[2]);
            result.dateOrTime = tokens[3];

            if (result.price <= 0.0) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "Price must be positive";
                return result;
            }
            return result;
        }

        if (rtype == "HIST") {
            // HIST|<ticker>|<YYYY-MM-DD>|<close:double>
            if (tokens.size() != 4) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "HIST requires 4 tokens, got " + std::to_string(tokens.size());
                return result;
            }
            result.type = ParseResultType::HIST;
            result.ticker = tokens[1];
            result.dateOrTime = tokens[2];
            result.price = std::stod(tokens[3]);

            if (result.price <= 0.0) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "Historical close price must be positive";
                return result;
            }
            return result;
        }

        if (rtype == "NEWS") {
            // NEWS|<ticker>|<YYYY-MM-DD>|<headline>
            if (tokens.size() < 4) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "NEWS requires at least 4 tokens";
                return result;
            }
            result.type = ParseResultType::NEWS;
            result.ticker = tokens[1];
            result.dateOrTime = tokens[2];
            result.headline = tokens[3];
            return result;
        }

        if (rtype == "BATCH_END") {
            if (tokens.size() != 2) {
                result.type = ParseResultType::MALFORMED;
                result.errorMsg = "BATCH_END requires 2 tokens";
                return result;
            }
            result.type = ParseResultType::BATCH_END;
            result.seq = std::stoi(tokens[1]);
            return result;
        }

        if (rtype == "BYE") {
            result.type = ParseResultType::BYE;
            return result;
        }

    } catch (const std::exception& ex) {
        result.type = ParseResultType::MALFORMED;
        result.errorMsg = std::string("Parsing exception on line ") + std::to_string(lineNumber) + ": " + ex.what();
        return result;
    }

    result.type = ParseResultType::UNKNOWN;
    result.errorMsg = "Unknown record type: " + rtype;
    return result;
}
