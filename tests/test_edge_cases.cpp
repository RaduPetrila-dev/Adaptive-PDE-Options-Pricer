#include <gtest/gtest.h>
#include <cmath>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

// --- Option input validation ---

TEST(OptionValidation, NegativeSpotThrows) {
    EXPECT_THROW(
        Option(-100, 100, 1.0, 0.05, 0.20, OptionType::Call),
        std::invalid_argument);
}

TEST(OptionValidation, ZeroStrikeThrows) {
    EXPECT_THROW(
        Option(100, 0, 1.0, 0.05, 0.20, OptionType::Call),
        std::invalid_argument);
}

TEST(OptionValidation, NegativeVolThrows) {
    EXPECT_THROW(
        Option(100, 100, 1.0, 0.05, -0.20, OptionType::Call),
        std::invalid_argument);
}

TEST(OptionValidation, ZeroMaturityThrows) {
    EXPECT_THROW(
        Option(100, 100, 0.0, 0.05, 0.20, OptionType::Call),
        std::invalid_argument);
}

// --- Payoff correctness ---

TEST(OptionPayoff, CallPayoff) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    EXPECT_DOUBLE_EQ(opt.payoff(120.0), 20.0);
    EXPECT_DOUBLE_EQ(opt.payoff(80.0), 0.0);
    EXPECT_DOUBLE_EQ(opt.payoff(100.0), 0.0);
}

TEST(OptionPayoff, PutPayoff) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Put);
    EXPECT_DOUBLE_EQ(opt.payoff(80.0), 20.0);
    EXPECT_DOUBLE_EQ(opt.payoff(120.0), 0.0);
    EXPECT_DOUBLE_EQ(opt.payoff(100.0), 0.0);
}

// --- Solver input validation ---

TEST(SolverValidation, TooFewSpaceStepsThrows) {
    EXPECT_THROW(PDESolver(5, 100, true), std::invalid_argument);
}

TEST(SolverValidation, ZeroTimeStepsThrows) {
    EXPECT_THROW(PDESolver(100, 0, true), std::invalid_argument);
}

// --- Price non-negativity ---

TEST(Sanity, EuropeanCallNonNegative) {
    Option opt(50, 100, 1.0, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceEuropean(opt), 0.0);
}

TEST(Sanity, EuropeanPutNonNegative) {
    Option opt(150, 100, 1.0, 0.05, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_GE(solver.priceEuropean(opt), 0.0);
}

// --- Convergence: finer grid should reduce error ---

TEST(Convergence, FinerGridReducesError) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    double bs = BlackScholes::price(opt);

    PDESolver coarse(50, 50, true);
    PDESolver fine(400, 400, true);

    double err_coarse = std::abs(coarse.priceEuropean(opt) - bs);
    double err_fine   = std::abs(fine.priceEuropean(opt) - bs);

    EXPECT_LT(err_fine, err_coarse);
}

// --- Adaptive grid reduces error vs uniform at same node count ---

TEST(Convergence, AdaptiveBetterThanUniform) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    double bs = BlackScholes::price(opt);

    PDESolver adaptive(150, 150, true);
    PDESolver uniform(150, 150, false);

    double err_adapt = std::abs(adaptive.priceEuropean(opt) - bs);
    double err_unif  = std::abs(uniform.priceEuropean(opt) - bs);

    EXPECT_LT(err_adapt, err_unif);
}
