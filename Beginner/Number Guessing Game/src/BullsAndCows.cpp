#include "BullsAndCows.hpp"
#include "RandomGenerator.hpp"
#include <unordered_set>
#include <cctype>
#include <algorithm>

namespace GuessGame {

    BullsAndCows::BullsAndCows(const std::string& secret) {
        if (secret.empty() || !isValidCode(secret)) {
            reset();
        } else {
            m_secret = secret;
        }
    }

    void BullsAndCows::reset() {
        m_secret = RandomGenerator::generateBullsCowsCode();
        m_history.clear();
    }

    void BullsAndCows::setSecret(const std::string& secret) {
        if (isValidCode(secret)) {
            m_secret = secret;
            m_history.clear();
        }
    }

    bool BullsAndCows::isValidCode(const std::string& code) {
        if (code.length() != 4) return false;
        std::unordered_set<char> seen;
        for (char ch : code) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) return false;
            if (seen.find(ch) != seen.end()) return false;
            seen.insert(ch);
        }
        return true;
    }

    BullsCowsResult BullsAndCows::evaluate(const std::string& guess, const std::string& secret) {
        BullsCowsResult result{0, 0};
        if (guess.length() != 4 || secret.length() != 4) return result;

        for (size_t i = 0; i < 4; ++i) {
            if (guess[i] == secret[i]) {
                result.bulls++;
            } else if (secret.find(guess[i]) != std::string::npos) {
                result.cows++;
            }
        }
        return result;
    }

    BullsCowsResult BullsAndCows::submitGuess(const std::string& guess) {
        BullsCowsResult res = evaluate(guess, m_secret);
        m_history.push_back({guess, res.bulls, res.cows});
        return res;
    }

    char BullsAndCows::revealRandomContainedDigit(const std::string& alreadyGuessedDigits) {
        std::vector<char> candidates;
        for (char ch : m_secret) {
            if (alreadyGuessedDigits.find(ch) == std::string::npos) {
                candidates.push_back(ch);
            }
        }
        if (candidates.empty()) return m_secret[0];
        int idx = RandomGenerator::getInt(0, static_cast<int>(candidates.size()) - 1);
        return candidates[idx];
    }

} // namespace GuessGame
