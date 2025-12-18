#include <gtest/gtest.h>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

TEST(EuropeanTest, ATMCall) {
    Option call(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Call);
    PDESolver solver(200, 100, true);
    EXPECT_NEAR(solver.priceEuropean(call), 
                BlackScholes::price(call), 0.01);
}
