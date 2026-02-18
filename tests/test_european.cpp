#include <gtest/gtest.h>
#include <cmath>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

// Tolerance for PDE vs Black-Scholes comparison.
// With 200 space x 200 time steps, errors should be well below this.
static constexpr double TOL = 0.05;

// --- ATM options ---

TEST(European, ATMCall) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, ATMPut) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- ITM options ---

TEST(European, DeepITMCall) {
    Option opt(150, 100, 1.0, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, ITMPut) {
    Option opt(80, 100, 1.0, 0.05, 0.25, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- OTM options ---

TEST(European, OTMCall) {
    Option opt(80, 100, 1.0, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, DeepOTMPut) {
    Option opt(150, 100, 0.5, 0.05, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- Short maturity ---

TEST(European, ShortMaturityCall) {
    Option opt(100, 100, 0.1, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, ShortMaturityPut) {
    Option opt(100, 100, 0.1, 0.05, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- Long maturity ---

TEST(European, LongMaturityCall) {
    Option opt(100, 100, 3.0, 0.05, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- High volatility ---

TEST(European, HighVolCall) {
    Option opt(100, 100, 1.0, 0.05, 0.50, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, HighVolPut) {
    Option opt(100, 100, 1.0, 0.05, 0.50, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- Low volatility ---

TEST(European, LowVolCall) {
    Option opt(100, 100, 1.0, 0.05, 0.10, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- Put-call parity: C - P = S - K*exp(-rT) ---

TEST(European, PutCallParity) {
    Option call(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    Option put(100, 100, 1.0, 0.05, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);

    double C = solver.priceEuropean(call);
    double P = solver.priceEuropean(put);
    double parity = call.S - call.K * std::exp(-call.r * call.T);

    EXPECT_NEAR(C - P, parity, TOL);
}

// --- Uniform vs adaptive: both should converge ---

TEST(European, UniformAlsoConverges) {
    Option opt(100, 100, 1.0, 0.05, 0.20, OptionType::Call);
    PDESolver uniform_solver(200, 200, false);
    EXPECT_NEAR(uniform_solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

// --- Varying rate ---

TEST(European, ZeroRateCall) {
    Option opt(100, 100, 1.0, 0.001, 0.20, OptionType::Call);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}

TEST(European, HighRatePut) {
    Option opt(100, 100, 1.0, 0.10, 0.20, OptionType::Put);
    PDESolver solver(200, 200, true);
    EXPECT_NEAR(solver.priceEuropean(opt), BlackScholes::price(opt), TOL);
}
