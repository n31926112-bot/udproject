/**
 * Projection.cpp - Implementation of CAGR and linear regression price projection.
 */
#include "Projection.h"
#include <cmath>
#include <algorithm>

ProjectionResult Projection::calculateOneYearEstimate(const TickerNode& ticker) {
    ProjectionResult res;
    res.symbol = ticker.symbol;
    res.currentPrice = ticker.latestPrice;
    res.disclaimer = "ESTIMATE ONLY - NOT A PREDICTION (Based on historical trend)";

    if (ticker.history.empty()) {
        res.estimatedPrice1Yr = ticker.latestPrice;
        res.expectedReturnPct = 0.0;
        res.method = "No Historical Data (Flat)";
        return res;
    }

    size_t n = ticker.history.size();
    double startPrice = ticker.history.front().close;
    double endPrice = ticker.history.back().close;

    if (n >= 30 && startPrice > 0.0 && endPrice > 0.0) {
        // Method 1: CAGR Calculation over available trading days (assume 250 trading days / year)
        double years = static_cast<double>(n) / 250.0;
        if (years <= 0.0) years = 1.0 / 250.0;

        double cagr = std::pow(endPrice / startPrice, 1.0 / years) - 1.0;
        
        // Clamp CAGR return between -80% and +200% for realistic boundary constraints
        cagr = std::max(-0.80, std::min(2.00, cagr));

        res.estimatedPrice1Yr = ticker.latestPrice * (1.0 + cagr);
        res.expectedReturnPct = cagr * 100.0;
        res.method = "CAGR over " + std::to_string(n) + " trading days";
    } else {
        // Method 2: Fallback to simple Linear Regression trend
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = ticker.history[i].close;
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumXX += x * x;
        }
        double denominator = (n * sumXX - sumX * sumX);
        double slope = (denominator != 0.0) ? (n * sumXY - sumX * sumY) / denominator : 0.0;

        double projectedDelta = slope * 250.0;
        res.estimatedPrice1Yr = std::max(0.01, ticker.latestPrice + projectedDelta);
        res.expectedReturnPct = (res.currentPrice > 0.0)
            ? ((res.estimatedPrice1Yr - res.currentPrice) / res.currentPrice) * 100.0
            : 0.0;
        
        // Clamp bounds
        res.expectedReturnPct = std::max(-80.0, std::min(200.0, res.expectedReturnPct));
        res.estimatedPrice1Yr = res.currentPrice * (1.0 + res.expectedReturnPct / 100.0);
        res.method = "Linear Trend Fallback (" + std::to_string(n) + " points)";
    }

    return res;
}
