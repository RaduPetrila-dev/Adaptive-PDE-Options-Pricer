#pragma once
#include <stdexcept>

enum class OptionType { Call, Put };
enum class ExerciseType { European, American };

class Option {
public:
    double S, K, T, r, sigma;
    OptionType type;
    ExerciseType exercise;

    Option(double spot, double strike, double maturity, 
           double rate, double vol, OptionType opt_type,
           ExerciseType ex_type = ExerciseType::European);

    double payoff(double spot) const;
private:
    void validate() const;
};
