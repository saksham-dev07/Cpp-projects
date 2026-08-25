#pragma once

#include "Types.hpp"
#include <random>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace rps {

class RandomGenerator {
public:
    static void init();
    static void setSeed(uint64_t seed);

    static int getInt(int minVal, int maxVal);
    static double getDouble(double minVal = 0.0, double maxVal = 1.0);
    static bool getBool(double trueProbability = 0.5);

    static Move getRandomMove(RuleSet ruleSet);
    static Move getWeightedMove(const std::vector<double>& weights, RuleSet ruleSet);

    template<typename T>
    static void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), s_engine);
    }

private:
    static inline std::mt19937_64 s_engine{std::random_device{}()};
};

} // namespace rps
