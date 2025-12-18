#include "Option.hpp"
#include <algorithm>
#include <cmath>

Option::Option(double spot, double strike, double maturity, 
               double rate, double vol, OptionType opt_type,
               ExerciseType ex_type)
    : S(spot), K(strike), T(maturity), r(rate), 
      sigma(vol), type(opt_type), exercise(ex_type) {
    validate();
}

void Option::validate() const {
    if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0)
        throw std::invalid_argument("Invalid parameters");
}

double Option::payoff(double spot) const {
    return (type == OptionType::Call) ? 
           std::max(spot - K, 0.0) : std::max(K - spot, 0.0);
}
