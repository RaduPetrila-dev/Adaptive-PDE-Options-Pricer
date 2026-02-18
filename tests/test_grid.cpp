#include <gtest/gtest.h>
#include <cmath>
#include "Grid.hpp"

// --- UniformGrid ---

TEST(UniformGrid, CorrectSize) {
    UniformGrid g(300.0, 100);
    EXPECT_EQ(g.size(), 101);  // M intervals = M+1 nodes
}

TEST(UniformGrid, BoundaryValues) {
    UniformGrid g(300.0, 100);
    EXPECT_DOUBLE_EQ(g.spot(0), 0.0);
    EXPECT_DOUBLE_EQ(g.spot(100), 300.0);
}

TEST(UniformGrid, EvenSpacing) {
    UniformGrid g(300.0, 100);
    double expected = 3.0;
    for (int i = 0; i < 100; ++i)
        EXPECT_NEAR(g.spacing(i), expected, 1e-12);
}

TEST(UniformGrid, FindIndex) {
    UniformGrid g(300.0, 100);
    // S = 150 should land at index 50 (150/3 = 50)
    EXPECT_EQ(g.findIndex(150.0), 50);
    EXPECT_EQ(g.findIndex(0.0), 0);
    EXPECT_EQ(g.findIndex(300.0), 99);
}

// --- AdaptiveGrid ---

TEST(AdaptiveGrid, BoundaryValues) {
    AdaptiveGrid g(300.0, 100, 100.0);
    EXPECT_DOUBLE_EQ(g.spot(0), 0.0);
    EXPECT_DOUBLE_EQ(g.spot(g.size() - 1), 300.0);
}

TEST(AdaptiveGrid, Monotonic) {
    AdaptiveGrid g(300.0, 200, 100.0);
    for (int i = 0; i < g.size() - 1; ++i)
        EXPECT_GT(g.spot(i + 1), g.spot(i));
}

TEST(AdaptiveGrid, FinerNearStrike) {
    // The spacing near the strike should be smaller than far from it.
    double K = 100.0;
    AdaptiveGrid g(300.0, 200, K);

    // Find spacing near strike vs far from strike.
    int idx_near = g.findIndex(K);
    int idx_far  = g.findIndex(10.0);  // far below strike

    double spacing_near = g.spacing(idx_near);
    double spacing_far  = g.spacing(idx_far);

    EXPECT_LT(spacing_near, spacing_far);
}

TEST(AdaptiveGrid, FindIndexConsistent) {
    AdaptiveGrid g(300.0, 200, 100.0);
    // For any spot, findIndex should bracket it.
    double S = 105.3;
    int i = g.findIndex(S);
    EXPECT_LE(g.spot(i), S);
    EXPECT_GT(g.spot(i + 1), S);
}

TEST(AdaptiveGrid, InvalidParametersThrow) {
    EXPECT_THROW(AdaptiveGrid(300.0, 5, 100.0), std::invalid_argument);
    EXPECT_THROW(AdaptiveGrid(-1.0, 100, 100.0), std::invalid_argument);
    EXPECT_THROW(AdaptiveGrid(300.0, 100, -1.0), std::invalid_argument);
}

TEST(UniformGrid, InvalidParametersThrow) {
    EXPECT_THROW(UniformGrid(300.0, 1), std::invalid_argument);
    EXPECT_THROW(UniformGrid(-1.0, 100), std::invalid_argument);
}
