#include <gtest/gtest.h>
#include <cmath>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

static constexpr double TOL = 0.05;

// --- American >= European (early exercise premium is non-negative) ---

TEST(American, PutGEEuropeanATM) {
    Option am(100, 100, 1.0, 0.05, 0.20, OptionType::Put, ExerciseType::American);
    Option eu(100, 100, 1.0, 0.05, 0.20, OptionType::Put, ExerciseType::European);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceAmerican(am), solver.priceEuropean(eu) - TOL);
}

TEST(American, PutGEEuropeanITM) {
    Option am(80, 100, 1.0, 0.05, 0.20, OptionType::Put, ExerciseType::American);
    Option eu(80, 100, 1.0, 0.05, 0.20, OptionType::Put, ExerciseType::European);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceAmerican(am), solver.priceEuropean(eu) - TOL);
}

TEST(American, CallGEEuropeanATM) {
    Option am(100, 100, 1.0, 0.05, 0.20, OptionType::Call, ExerciseType::American);
    Option eu(100, 100, 1.0, 0.05, 0.20, OptionType::Call, ExerciseType::European);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceAmerican(am), solver.priceEuropean(eu) - TOL);
}

// --- American put >= intrinsic value ---

TEST(American, PutGEIntrinsicDeepITM) {
    Option am(60, 100, 1.0, 0.05, 0.20, OptionType::Put, ExerciseType::American);
    PDESolver solver(200, 200, true);
    double intrinsic = am.payoff(am.S);  // 40.0
    EXPECT_GE(solver.priceAmerican(am), intrinsic - TOL);
}

// --- American call on non-dividend stock ≈ European call ---

TEST(American, CallEqualsEuropeanNoDividend) {
    Option am(100, 100, 1.0, 0.05, 0.20, OptionType::Call, ExerciseType::American);
    Option eu(100, 100, 1.0, 0.05, 0.20, OptionType::Call, ExerciseType::European);
    PDESolver solver(200, 200, true);
    // Without dividends, American call = European call.
    EXPECT_NEAR(solver.priceAmerican(am), solver.priceEuropean(eu), TOL);
}

// --- American put with different parameters ---

TEST(American, HighVolPut) {
    Option am(100, 100, 1.0, 0.05, 0.50, OptionType::Put, ExerciseType::American);
    Option eu(100, 100, 1.0, 0.05, 0.50, OptionType::Put, ExerciseType::European);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceAmerican(am), solver.priceEuropean(eu) - TOL);
}

TEST(American, ShortMaturityPut) {
    Option am(100, 100, 0.25, 0.05, 0.20, OptionType::Put, ExerciseType::American);
    Option eu(100, 100, 0.25, 0.05, 0.20, OptionType::Put, ExerciseType::European);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceAmerican(am), solver.priceEuropean(eu) - TOL);
}

TEST(American, HighRatePut) {
    // High interest rate increases early exercise premium for puts.
    Option am(100, 100, 1.0, 0.10, 0.20, OptionType::Put, ExerciseType::American);
    Option eu(100, 100, 1.0, 0.10, 0.20, OptionType::Put, ExerciseType::European);
    PDESolver solver(200, 200, true);
    double am_price = solver.priceAmerican(am);
    double eu_price = solver.priceEuropean(eu);
    // Premium should be strictly positive at high rates.
    EXPECT_GT(am_price, eu_price + 0.01);
}
