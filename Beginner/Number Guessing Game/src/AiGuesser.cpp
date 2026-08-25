#include "AiGuesser.hpp"
#include "RandomGenerator.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace GuessGame {

    AiGuesser::AiGuesser(int minVal, int maxVal, AiStrategy strategy) {
        reset(minVal, maxVal, strategy);
    }

    void AiGuesser::reset(int minVal, int maxVal, AiStrategy strategy) {
        if (minVal > maxVal) std::swap(minVal, maxVal);
        m_originalMin = minVal;
        m_originalMax = maxVal;
        m_low = minVal;
        m_high = maxVal;
        m_strategy = strategy;
        m_contradiction = false;
        m_contradictionReason.clear();
        m_history.clear();
    }

    int AiGuesser::makeGuess() {
        if (m_low > m_high) {
            m_contradiction = true;
            return m_low;
        }

        if (m_low == m_high) {
            return m_low;
        }

        switch (m_strategy) {
            case AiStrategy::BinarySearch: {
                // Optimal binary search midpoint
                return m_low + (m_high - m_low) / 2;
            }

            case AiStrategy::Heuristic: {
                // Midpoint with small human-like deviation
                int mid = m_low + (m_high - m_low) / 2;
                int span = m_high - m_low;
                int maxDelta = std::max(1, span / 8);
                int delta = RandomGenerator::getInt(-maxDelta, maxDelta);
                int guess = std::clamp(mid + delta, m_low, m_high);
                return guess;
            }

            case AiStrategy::NoviceRandom: {
                // Uniform random within remaining valid bounds
                return RandomGenerator::getInt(m_low, m_high);
            }
        }

        return m_low + (m_high - m_low) / 2;
    }

    bool AiGuesser::processFeedback(int guess, char feedback) {
        AiGuessStep step;
        step.guess = guess;
        step.feedback = feedback;
        step.lowBound = m_low;
        step.highBound = m_high;
        m_history.push_back(step);

        if (feedback == 'C' || feedback == 'c') {
            return true;
        }

        if (feedback == 'H' || feedback == 'h') {
            // Target is higher than guess
            int newLow = guess + 1;
            if (newLow > m_high) {
                m_contradiction = true;
                std::stringstream ss;
                ss << "Contradiction detected! You told me earlier the number was <= " << m_high
                   << ", but now you say it is HIGHER than " << guess << "! There are no integers left in ["
                   << newLow << ".." << m_high << "].";
                m_contradictionReason = ss.str();
                return false;
            }
            m_low = std::max(m_low, newLow);
        } else if (feedback == 'L' || feedback == 'l') {
            // Target is lower than guess
            int newHigh = guess - 1;
            if (newHigh < m_low) {
                m_contradiction = true;
                std::stringstream ss;
                ss << "Contradiction detected! You told me earlier the number was >= " << m_low
                   << ", but now you say it is LOWER than " << guess << "! There are no integers left in ["
                   << m_low << ".." << newHigh << "].";
                m_contradictionReason = ss.str();
                return false;
            }
            m_high = std::min(m_high, newHigh);
        }

        if (m_low > m_high) {
            m_contradiction = true;
            std::stringstream ss;
            ss << "Logical anomaly: The valid search interval collapsed to [" << m_low << ".." << m_high << "]!";
            m_contradictionReason = ss.str();
            return false;
        }

        return true;
    }

    int AiGuesser::getRemainingPossibilities() const noexcept {
        if (m_low > m_high) return 0;
        return m_high - m_low + 1;
    }

} // namespace GuessGame
