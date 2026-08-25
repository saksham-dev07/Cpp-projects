#pragma once

#include "Types.hpp"
#include <string>
#include <vector>
#include <utility>

namespace GuessGame {

    struct HintDetail {
        int id = 0;
        std::string name;
        std::string description;
        int cost = 0;
        bool revealed = false;
    };

    class HintSystem {
    public:
        static bool isPrime(int n);
        static std::vector<int> getFactors(int n);
        static int getDigitSum(int n);
        static int getDigitCount(int n);
        static bool isEven(int n) noexcept { return n % 2 == 0; }

        static Proximity calculateProximity(int guess, int target, int minVal, int maxVal);
        static std::vector<HintDetail> generateHintsForTarget(int target, int minVal, int maxVal);
        static std::pair<int, int> getNarrowedBounds(int target, int curMin, int curMax, double factor = 0.35);
    };

} // namespace GuessGame
