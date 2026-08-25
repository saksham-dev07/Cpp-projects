#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace GuessGame {

    struct BullsCowsHistoryEntry {
        std::string guess;
        int bulls = 0;
        int cows = 0;
    };

    class BullsAndCows {
    public:
        explicit BullsAndCows(const std::string& secret = "");

        void reset();
        void setSecret(const std::string& secret);
        const std::string& getSecret() const noexcept { return m_secret; }

        static bool isValidCode(const std::string& code);
        static BullsCowsResult evaluate(const std::string& guess, const std::string& secret);

        BullsCowsResult submitGuess(const std::string& guess);
        const std::vector<BullsCowsHistoryEntry>& getHistory() const noexcept { return m_history; }
        char revealRandomContainedDigit(const std::string& alreadyGuessedDigits);

    private:
        std::string m_secret;
        std::vector<BullsCowsHistoryEntry> m_history;
    };

} // namespace GuessGame
