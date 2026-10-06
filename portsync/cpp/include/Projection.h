/**
 * Projection.h - One-year portfolio projection estimator using CAGR and linear regression.
 * Demonstrates: Honest labelling ("ESTIMATE"), statistical trend analysis, boundary clamping.
 */
#ifndef PROJECTION_H
#define PROJECTION_H

#include "Ticker.h"
#include <string>

struct ProjectionResult {
    std::string symbol;
    double currentPrice;
    double estimatedPrice1Yr;
    double expectedReturnPct;
    std::string method;
    std::string disclaimer;

    ProjectionResult()
        : currentPrice(0.0),
          estimatedPrice1Yr(0.0),
          expectedReturnPct(0.0),
          method("N/A"),
          disclaimer("ESTIMATE ONLY - NOT A PREDICTION") {}
};

class Projection {
public:
    static ProjectionResult calculateOneYearEstimate(const TickerNode& ticker);
};

#endif // PROJECTION_H
