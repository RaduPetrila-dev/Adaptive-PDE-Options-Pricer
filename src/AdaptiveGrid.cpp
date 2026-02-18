#include "Grid.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

AdaptiveGrid::AdaptiveGrid(double S_max, int M_total, double K,
                           double frac, double width) {
    if (M_total < 10 || S_max <= 0.0 || K <= 0.0)
        throw std::invalid_argument("AdaptiveGrid: invalid parameters");

    double half_w = width * K;
    double lo = std::max(0.0, K - half_w);
    double hi = std::min(S_max, K + half_w);

    // Allocate intervals: frac goes to the refinement zone, rest to outer.
    int M_mid = std::max(4, static_cast<int>(frac * M_total));
    int M_outer = M_total - M_mid;
    double outer_len = lo + (S_max - hi);

    int M_lo = 1;
    int M_hi = 1;
    if (outer_len > 0.0) {
        M_lo = std::max(1, static_cast<int>(M_outer * lo / outer_len));
        M_hi = std::max(1, M_outer - M_lo);
    }

    nodes_.reserve(M_lo + M_mid + M_hi + 1);

    // Helper: push evenly spaced nodes for [a, b) with n intervals.
    auto add_region = [&](double a, double b, int n) {
        double step = (b - a) / n;
        for (int i = 0; i < n; ++i)
            nodes_.push_back(a + i * step);
    };

    if (lo > 0.0) add_region(0.0, lo, M_lo);
    add_region(lo, hi, M_mid);
    add_region(hi, S_max, M_hi);
    nodes_.push_back(S_max);

    // Remove accidental duplicates at region boundaries.
    auto last = std::unique(nodes_.begin(), nodes_.end(),
        [](double a, double b) { return std::abs(a - b) < 1e-14; });
    nodes_.erase(last, nodes_.end());
}
