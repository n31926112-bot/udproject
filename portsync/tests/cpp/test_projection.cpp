/**
 * test_projection.cpp - Unit tests for Projection module (CAGR and linear trend fallback).
 */
#include "Projection.h"
#include <cassert>
#include <iostream>
#include <cmath>

void testCAGRProjection() {
    TickerNode ticker("RELIANCE");
    ticker.latestPrice = 2500.0;

    // Simulate 250 days of growth from 2000.0 to 2500.0 (25% annual growth)
    for (int i = 0; i < 250; ++i) {
        double price = 2000.0 + (500.0 * i / 249.0);
        ticker.history.emplace_back("2023-01-01", price);
    }

    ProjectionResult res = Projection::calculateOneYearEstimate(ticker);
    assert(res.symbol == "RELIANCE");
    assert(res.method.find("CAGR") != std::string::npos);
    assert(res.disclaimer.find("ESTIMATE ONLY") != std::string::npos);
    assert(res.expectedReturnPct > 20.0 && res.expectedReturnPct < 30.0);

    std::cout << "[PASS] testCAGRProjection\n";
}

void testLinearFallbackProjection() {
    TickerNode ticker("INFY");
    ticker.latestPrice = 1400.0;

    // Simulate under 30 days (e.g. 15 days)
    for (int i = 0; i < 15; ++i) {
        ticker.history.emplace_back("2023-01-01", 1350.0 + i * 2.0);
    }

    ProjectionResult res = Projection::calculateOneYearEstimate(ticker);
    assert(res.symbol == "INFY");
    assert(res.method.find("Linear Trend") != std::string::npos);
    assert(res.disclaimer.find("ESTIMATE ONLY") != std::string::npos);

    std::cout << "[PASS] testLinearFallbackProjection\n";
}

int main() {
    std::cout << "--- Running Projection Unit Tests ---\n";
    testCAGRProjection();
    testLinearFallbackProjection();
    std::cout << "All Projection tests passed successfully!\n";
    return 0;
}
