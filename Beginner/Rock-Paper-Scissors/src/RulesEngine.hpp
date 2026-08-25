#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace rps {

class RulesEngine {
public:
    static Outcome evaluate(Move p1, Move p2, RuleSet ruleSet);
    static std::string getVerb(Move winner, Move loser);
    static std::string getRoundDescription(Move p1, Move p2, Outcome outcome,
                                           const std::string& p1Name = "Player",
                                           const std::string& p2Name = "AI");

    static std::vector<Move> getWinningCounters(Move targetMove, RuleSet ruleSet);
    static std::vector<Move> getLosingVictims(Move myMove, RuleSet ruleSet);
    static bool isLegalMove(Move move, RuleSet ruleSet);
    static std::vector<Move> getLegalMoves(RuleSet ruleSet);
};

} // namespace rps
