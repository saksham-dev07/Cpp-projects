#include "HintSystem.hpp"
#include <cmath>
#include <sstream>
#include <algorithm>

namespace GuessGame {

    bool HintSystem::isPrime(int n) {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;
        for (int i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }

    std::vector<int> HintSystem::getFactors(int n) {
        std::vector<int> factors;
        if (n <= 0) return factors;
        for (int i = 1; i * i <= n; ++i) {
            if (n % i == 0) {
                factors.push_back(i);
                if (i * i != n) {
                    factors.push_back(n / i);
                }
            }
        }
        std::sort(factors.begin(), factors.end());
        return factors;
    }

    int HintSystem::getDigitSum(int n) {
        int sum = 0;
        n = std::abs(n);
        while (n > 0) {
            sum += n % 10;
            n /= 10;
        }
        return sum;
    }

    int HintSystem::getDigitCount(int n) {
        if (n == 0) return 1;
        int count = 0;
        n = std::abs(n);
        while (n > 0) {
            count++;
            n /= 10;
        }
        return count;
    }

    Proximity HintSystem::calculateProximity(int guess, int target, int minVal, int maxVal) {
        int diff = std::abs(guess - target);
        if (diff == 0) return Proximity::Boiling;

        int totalRange = std::max(1, std::abs(maxVal - minVal));
        double ratio = static_cast<double>(diff) / totalRange;

        if (diff <= 2 || ratio <= 0.03) return Proximity::Boiling;
        if (diff <= 5 || ratio <= 0.08) return Proximity::Burning;
        if (diff <= 12 || ratio <= 0.18) return Proximity::Hot;
        if (diff <= 25 || ratio <= 0.35) return Proximity::Warm;
        if (diff <= 50 || ratio <= 0.55) return Proximity::Cold;
        return Proximity::Freezing;
    }

    std::vector<HintDetail> HintSystem::generateHintsForTarget(int target, int minVal, int maxVal) {
        std::vector<HintDetail> hints;

        // Hint 1: Parity
        {
            HintDetail h;
            h.id = 1;
            h.name = "Parity Check";
            h.description = isEven(target) ? "The secret number is EVEN." : "The secret number is ODD.";
            h.cost = 50;
            h.revealed = false;
            hints.push_back(h);
        }

        // Hint 2: Prime status
        {
            HintDetail h;
            h.id = 2;
            h.name = "Primality Test";
            h.description = isPrime(target) ? "The secret number is a PRIME number." : "The secret number is a COMPOSITE number.";
            h.cost = 75;
            h.revealed = false;
            hints.push_back(h);
        }

        // Hint 3: Divisibility / Factor clue
        {
            HintDetail h;
            h.id = 3;
            h.name = "Divisibility Clue";
            std::stringstream ss;
            if (target % 10 == 0) {
                ss << "The number is a multiple of 10 (ends in 0).";
            } else if (target % 5 == 0) {
                ss << "The number is divisible by 5.";
            } else if (target % 3 == 0) {
                ss << "The number is divisible by 3.";
            } else if (target % 7 == 0) {
                ss << "The number is divisible by 7.";
            } else {
                auto factors = getFactors(target);
                if (factors.size() > 2) {
                    // Pick a non-trivial factor
                    int nontrivial = factors[1];
                    ss << "The number is divisible by " << nontrivial << ".";
                } else {
                    ss << "The number has NO small prime factors under 10 (it's coprime to 2, 3, 5, 7).";
                }
            }
            h.description = ss.str();
            h.cost = 100;
            h.revealed = false;
            hints.push_back(h);
        }

        // Hint 4: Digit sum
        {
            HintDetail h;
            h.id = 4;
            h.name = "Sum of Digits";
            std::stringstream ss;
            ss << "The sum of all its digits equals " << getDigitSum(target) << " (Total digits: " << getDigitCount(target) << ").";
            h.description = ss.str();
            h.cost = 120;
            h.revealed = false;
            hints.push_back(h);
        }

        // Hint 5: Bounds narrowing
        {
            HintDetail h;
            h.id = 5;
            h.name = "Radar Shrink";
            auto [nMin, nMax] = getNarrowedBounds(target, minVal, maxVal, 0.40);
            std::stringstream ss;
            ss << "Target is narrowed to the sub-interval: [" << nMin << " .. " << nMax << "].";
            h.description = ss.str();
            h.cost = 150;
            h.revealed = false;
            hints.push_back(h);
        }

        return hints;
    }

    std::pair<int, int> HintSystem::getNarrowedBounds(int target, int curMin, int curMax, double factor) {
        int span = curMax - curMin;
        int halfSpan = std::max(1, static_cast<int>(span * factor / 2.0));
        int newMin = std::max(curMin, target - halfSpan);
        int newMax = std::min(curMax, target + halfSpan);
        return { newMin, newMax };
    }

} // namespace GuessGame
