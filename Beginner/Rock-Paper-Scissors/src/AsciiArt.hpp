#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace rps {

class AsciiArt {
public:
    static void printMainBanner();
    static void printRulesDiagram(RuleSet ruleSet);
    
    static void playCountdown(RuleSet ruleSet, bool fast = false);

    static void renderClash(Move p1Move, Move p2Move, Outcome outcome,
                            const std::string& p1Name = "Player",
                            const std::string& p2Name = "AI");

    static void printVictoryBanner();
    static void printDefeatBanner();
    static void printDrawBanner();

    static std::vector<std::string> getHandArt(Move move, bool facingRight);
};

} // namespace rps
