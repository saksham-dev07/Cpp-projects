#pragma once

#include <random>
#include <string>
#include <vector>

namespace GuessGame {

    class RandomGenerator {
    public:
        static int getInt(int minVal, int maxVal);
        static double getDouble(double minVal, double maxVal);
        static std::string generateBullsCowsCode();
        static int computeOptimalAttempts(int minVal, int maxVal);

        static void seed(unsigned int s);
        static void resetSeed();

    private:
        static std::mt19937 s_engine;
        static bool s_deterministic;
    };

} // namespace GuessGame
