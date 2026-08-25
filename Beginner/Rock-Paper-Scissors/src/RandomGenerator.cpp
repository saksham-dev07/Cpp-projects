#include "RandomGenerator.hpp"
#include <chrono>

namespace rps {

void RandomGenerator::init() {
    uint64_t seed = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    s_engine.seed(seed);
}

void RandomGenerator::setSeed(uint64_t seed) {
    s_engine.seed(seed);
}

int RandomGenerator::getInt(int minVal, int maxVal) {
    if (minVal >= maxVal) return minVal;
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(s_engine);
}

double RandomGenerator::getDouble(double minVal, double maxVal) {
    if (minVal >= maxVal) return minVal;
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(s_engine);
}

bool RandomGenerator::getBool(double trueProbability) {
    if (trueProbability <= 0.0) return false;
    if (trueProbability >= 1.0) return true;
    std::bernoulli_distribution dist(trueProbability);
    return dist(s_engine);
}

Move RandomGenerator::getRandomMove(RuleSet ruleSet) {
    int maxIndex = (ruleSet == RuleSet::Classic) ? 3 : 5;
    return static_cast<Move>(getInt(1, maxIndex));
}

Move RandomGenerator::getWeightedMove(const std::vector<double>& weights, RuleSet ruleSet) {
    size_t count = (ruleSet == RuleSet::Classic) ? 3 : 5;
    if (weights.size() < count) {
        return getRandomMove(ruleSet);
    }

    std::discrete_distribution<size_t> dist(weights.begin(), weights.begin() + count);
    return static_cast<Move>(dist(s_engine) + 1);
}

} // namespace rps
