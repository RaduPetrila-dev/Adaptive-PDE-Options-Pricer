#pragma once
#include <vector>

// Spatial grid for the PDE domain [0, S_max].
// Stores non-uniform node positions and provides local spacing.
class Grid {
public:
    virtual ~Grid() = default;

    int size() const;
    double spot(int i) const;
    double spacing(int i) const;       // h_i = S_{i+1} - S_i
    int findIndex(double S) const;     // index i where nodes[i] <= S < nodes[i+1]
    const std::vector<double>& nodes() const;

protected:
    std::vector<double> nodes_;
};

// Evenly spaced grid: S_i = i * (S_max / M).
class UniformGrid : public Grid {
public:
    UniformGrid(double S_max, int M);
};

// Adaptive grid concentrating points near the strike price.
//
// Three-region piecewise uniform grid:
//   [0, K - w]       coarse spacing
//   [K - w, K + w]   fine spacing  (refinement zone around the payoff kink)
//   [K + w, S_max]   coarse spacing
//
// A larger share of the node budget goes to the refinement zone,
// producing smaller spacing where the solution gradient is steepest.
class AdaptiveGrid : public Grid {
public:
    // M_total  = total number of spatial intervals.
    // S_max    = upper boundary.
    // K        = strike (centre of refinement).
    // frac     = fraction of intervals in the refinement zone (default 0.60).
    // width    = half-width of the zone as a fraction of K (default 0.25).
    AdaptiveGrid(double S_max, int M_total, double K,
                 double frac = 0.60, double width = 0.25);
};
