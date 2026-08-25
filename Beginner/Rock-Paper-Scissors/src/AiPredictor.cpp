#include "AiPredictor.hpp"
#include "RandomGenerator.hpp"
#include "RulesEngine.hpp"
#include <algorithm>

namespace rps {

std::unique_ptr<AiPredictor> AiPredictor::create(AiPersonality personality) {
    switch (personality) {
        case AiPersonality::Random:        return std::make_unique<RandomAi>();
        case AiPersonality::Brute:         return std::make_unique<BruteAi>();
        case AiPersonality::Mimic:         return std::make_unique<MimicAi>();
        case AiPersonality::TacticianWSLS: return std::make_unique<WslsAi>();
        case AiPersonality::MarkovOracle:  return std::make_unique<MarkovOracleAi>();
        default:                           return std::make_unique<RandomAi>();
    }
}

// ----------------------------------------------------
// RandomAi
// ----------------------------------------------------
Move RandomAi::selectMove(RuleSet ruleSet) {
    return RandomGenerator::getRandomMove(ruleSet);
}

// ----------------------------------------------------
// BruteAi
// ----------------------------------------------------
Move BruteAi::selectMove(RuleSet ruleSet) {
    if (ruleSet == RuleSet::Classic) {
        // Rock: 55%, Paper: 20%, Scissors: 25%
        return RandomGenerator::getWeightedMove({ 0.55, 0.20, 0.25 }, ruleSet);
    }
    // Rock: 45%, Spock: 20%, Scissors: 15%, Lizard: 10%, Paper: 10%
    return RandomGenerator::getWeightedMove({ 0.45, 0.10, 0.15, 0.10, 0.20 }, ruleSet);
}

// ----------------------------------------------------
// MimicAi
// ----------------------------------------------------
void MimicAi::reset() {
    m_lastPlayerMove = Move::None;
}

void MimicAi::recordRound(Move playerMove, Move /*aiMove*/, Outcome /*outcome*/) {
    m_lastPlayerMove = playerMove;
}

Move MimicAi::selectMove(RuleSet ruleSet) {
    if (m_lastPlayerMove == Move::None) {
        return RandomGenerator::getRandomMove(ruleSet);
    }

    // 60% mimic last move, 40% counter it
    if (RandomGenerator::getBool(0.60)) {
        if (RulesEngine::isLegalMove(m_lastPlayerMove, ruleSet)) {
            return m_lastPlayerMove;
        }
    } else {
        auto counters = RulesEngine::getWinningCounters(m_lastPlayerMove, ruleSet);
        if (!counters.empty()) {
            int idx = RandomGenerator::getInt(0, static_cast<int>(counters.size()) - 1);
            return counters[idx];
        }
    }

    return RandomGenerator::getRandomMove(ruleSet);
}

// ----------------------------------------------------
// WslsAi (Win-Stay, Lose-Shift)
// ----------------------------------------------------
void WslsAi::reset() {
    m_lastPlayerMove = Move::None;
    m_lastAiMove = Move::None;
    m_lastOutcome = Outcome::Tie;
}

void WslsAi::recordRound(Move playerMove, Move aiMove, Outcome outcome) {
    m_lastPlayerMove = playerMove;
    m_lastAiMove = aiMove;
    m_lastOutcome = outcome;
}

Move WslsAi::selectMove(RuleSet ruleSet) {
    if (m_lastPlayerMove == Move::None || m_lastAiMove == Move::None) {
        return RandomGenerator::getRandomMove(ruleSet);
    }

    if (m_lastOutcome == Outcome::Player1Win) {
        // Player won: expect them to stay with the winning move -> counter it
        auto counters = RulesEngine::getWinningCounters(m_lastPlayerMove, ruleSet);
        if (!counters.empty()) {
            return counters[RandomGenerator::getInt(0, static_cast<int>(counters.size()) - 1)];
        }
    } else if (m_lastOutcome == Outcome::Player2Win) {
        // Player lost: expect them to shift to the move that beats what AI just played
        auto playerShiftMoves = RulesEngine::getWinningCounters(m_lastAiMove, ruleSet);
        if (!playerShiftMoves.empty()) {
            Move anticipated = playerShiftMoves[RandomGenerator::getInt(0, static_cast<int>(playerShiftMoves.size()) - 1)];
            auto aiCounters = RulesEngine::getWinningCounters(anticipated, ruleSet);
            if (!aiCounters.empty()) {
                return aiCounters[RandomGenerator::getInt(0, static_cast<int>(aiCounters.size()) - 1)];
            }
        }
    } else {
        // Tie: player often shifts to counter the tied move
        auto counters = RulesEngine::getWinningCounters(m_lastPlayerMove, ruleSet);
        if (!counters.empty()) {
            Move anticipated = counters[0];
            auto aiCounters = RulesEngine::getWinningCounters(anticipated, ruleSet);
            if (!aiCounters.empty()) {
                return aiCounters[RandomGenerator::getInt(0, static_cast<int>(aiCounters.size()) - 1)];
            }
        }
    }

    return RandomGenerator::getRandomMove(ruleSet);
}

// ----------------------------------------------------
// MarkovOracleAi
// ----------------------------------------------------
MarkovOracleAi::MarkovOracleAi() {
    reset();
}

void MarkovOracleAi::reset() {
    m_playerHistory.clear();
    m_aiHistory.clear();

    for (auto& row : m_firstOrderMatrix) {
        row.fill(0.2); // Laplace smoothing
    }
    m_secondOrderMatrix.clear();
}

void MarkovOracleAi::recordRound(Move playerMove, Move aiMove, Outcome /*outcome*/) {
    if (playerMove == Move::None) return;

    // Decay older observations
    for (auto& row : m_firstOrderMatrix) {
        for (double& val : row) {
            val *= m_decay;
        }
    }

    for (auto& pair : m_secondOrderMatrix) {
        for (double& val : pair.second) {
            val *= m_decay;
        }
    }

    // 1st-order update
    if (!m_playerHistory.empty()) {
        uint8_t prev = static_cast<uint8_t>(m_playerHistory.back());
        uint8_t curr = static_cast<uint8_t>(playerMove);
        if (prev < 6 && curr < 6) {
            m_firstOrderMatrix[prev][curr] += 1.0;
        }
    }

    // 2nd-order update
    if (m_playerHistory.size() >= 2) {
        uint8_t prev2 = static_cast<uint8_t>(m_playerHistory[m_playerHistory.size() - 2]);
        uint8_t prev1 = static_cast<uint8_t>(m_playerHistory.back());
        uint8_t curr  = static_cast<uint8_t>(playerMove);

        uint32_t key = (static_cast<uint32_t>(prev2) << 8) | static_cast<uint32_t>(prev1);
        if (m_secondOrderMatrix.find(key) == m_secondOrderMatrix.end()) {
            m_secondOrderMatrix[key].fill(0.1);
        }
        if (curr < 6) {
            m_secondOrderMatrix[key][curr] += 1.0;
        }
    }

    m_playerHistory.push_back(playerMove);
    m_aiHistory.push_back(aiMove);
}

double MarkovOracleAi::getTransitionProbability(Move prevMove, Move nextMove, RuleSet ruleSet) const {
    uint8_t p = static_cast<uint8_t>(prevMove);
    uint8_t n = static_cast<uint8_t>(nextMove);
    if (p >= 6 || n >= 6) return 0.0;

    double total = 0.0;
    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;
    for (int i = 1; i <= maxIdx; ++i) {
        total += m_firstOrderMatrix[p][i];
    }
    if (total <= 0.0001) return (ruleSet == RuleSet::Classic) ? 0.333 : 0.20;

    return m_firstOrderMatrix[p][n] / total;
}

Move MarkovOracleAi::predictMostLikelyPlayerMove(RuleSet ruleSet) const {
    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;

    if (m_playerHistory.empty()) {
        return RandomGenerator::getRandomMove(ruleSet);
    }

    // Try 2nd-order model first
    if (m_playerHistory.size() >= 2) {
        uint8_t prev2 = static_cast<uint8_t>(m_playerHistory[m_playerHistory.size() - 2]);
        uint8_t prev1 = static_cast<uint8_t>(m_playerHistory.back());
        uint32_t key = (static_cast<uint32_t>(prev2) << 8) | static_cast<uint32_t>(prev1);

        auto it = m_secondOrderMatrix.find(key);
        if (it != m_secondOrderMatrix.end()) {
            double bestScore = -1.0;
            int bestMove = 1;
            for (int i = 1; i <= maxIdx; ++i) {
                if (it->second[i] > bestScore) {
                    bestScore = it->second[i];
                    bestMove = i;
                }
            }
            if (bestScore > 0.5) {
                return static_cast<Move>(bestMove);
            }
        }
    }

    // Fall back to 1st-order
    uint8_t lastMove = static_cast<uint8_t>(m_playerHistory.back());
    if (lastMove >= 1 && lastMove <= 5) {
        double bestScore = -1.0;
        int bestMove = 1;
        for (int i = 1; i <= maxIdx; ++i) {
            if (m_firstOrderMatrix[lastMove][i] > bestScore) {
                bestScore = m_firstOrderMatrix[lastMove][i];
                bestMove = i;
            }
        }
        return static_cast<Move>(bestMove);
    }

    return RandomGenerator::getRandomMove(ruleSet);
}

Move MarkovOracleAi::selectMove(RuleSet ruleSet) {
    if (m_playerHistory.empty()) {
        return RandomGenerator::getRandomMove(ruleSet);
    }

    // Epsilon-greedy exploration
    if (RandomGenerator::getBool(m_epsilon)) {
        return RandomGenerator::getRandomMove(ruleSet);
    }

    Move predicted = predictMostLikelyPlayerMove(ruleSet);
    auto counters = RulesEngine::getWinningCounters(predicted, ruleSet);
    if (!counters.empty()) {
        int idx = RandomGenerator::getInt(0, static_cast<int>(counters.size()) - 1);
        return counters[idx];
    }

    return RandomGenerator::getRandomMove(ruleSet);
}

} // namespace rps
