#include "RandomGenerator.hpp"
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace GuessGame {

    static std::mt19937 initEngine() {
        std::random_device rd;
        return std::mt19937(rd());
    }

    std::mt19937 RandomGenerator::s_engine = initEngine();
    bool RandomGenerator::s_deterministic = false;

    void RandomGenerator::seed(unsigned int s) {
        s_engine.seed(s);
        s_deterministic = true;
    }

    void RandomGenerator::resetSeed() {
        std::random_device rd;
        s_engine.seed(rd());
        s_deterministic = false;
    }

    int RandomGenerator::getInt(int minVal, int maxVal) {
        if (minVal > maxVal) std::swap(minVal, maxVal);
        std::uniform_int_distribution<int> dist(minVal, maxVal);
        return dist(s_engine);
    }

    double RandomGenerator::getDouble(double minVal, double maxVal) {
        if (minVal > maxVal) std::swap(minVal, maxVal);
        std::uniform_real_distribution<double> dist(minVal, maxVal);
        return dist(s_engine);
    }

    std::string RandomGenerator::generateBullsCowsCode() {
        std::vector<char> digits = {'0','1','2','3','4','5','6','7','8','9'};
        std::shuffle(digits.begin(), digits.end(), s_engine);
        std::string code;
        for (int i = 0; i < 4; ++i) {
            code += digits[i];
        }
        return code;
    }

    int RandomGenerator::computeOptimalAttempts(int minVal, int maxVal) {
        int range = std::abs(maxVal - minVal) + 1;
        if (range <= 1) return 1;
        return static_cast<int>(std::ceil(std::log2(static_cast<double>(range))));
    }

} // namespace GuessGame
