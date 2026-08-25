#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace GuessGame {

    struct AiGuessStep {
        int guess = 0;
        char feedback = ' ';
        int lowBound = 0;
        int highBound = 0;
    };

    class AiGuesser {
    public:
        explicit AiGuesser(int minVal = 1, int maxVal = 100, AiStrategy strategy = AiStrategy::BinarySearch);

        void reset(int minVal, int maxVal, AiStrategy strategy = AiStrategy::BinarySearch);
        int makeGuess();
        bool processFeedback(int guess, char feedback);

        bool hasContradiction() const noexcept { return m_contradiction; }
        std::string getContradictionExplanation() const { return m_contradictionReason; }

        int getLowBound() const noexcept { return m_low; }
        int getHighBound() const noexcept { return m_high; }
        int getRemainingPossibilities() const noexcept;
        const std::vector<AiGuessStep>& getHistory() const noexcept { return m_history; }

    private:
        int m_originalMin = 1;
        int m_originalMax = 100;
        int m_low = 1;
        int m_high = 100;
        AiStrategy m_strategy = AiStrategy::BinarySearch;
        bool m_contradiction = false;
        std::string m_contradictionReason;
        std::vector<AiGuessStep> m_history;
    };

} // namespace GuessGame
