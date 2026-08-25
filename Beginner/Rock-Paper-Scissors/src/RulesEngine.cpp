#include "RulesEngine.hpp"
#include "InputValidator.hpp"
#include <sstream>

namespace rps {

namespace {
    // 0 = Tie, 1 = Move A beats Move B, 2 = Move B beats Move A
    // Indices: [1=Rock, 2=Paper, 3=Scissors, 4=Lizard, 5=Spock]
    constexpr uint8_t kRpslsMatrix[6][6] = {
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 2, 1, 1, 2 }, // 1: Rock vs [R, P, S, L, K]
        { 0, 1, 0, 2, 2, 1 }, // 2: Paper vs [R, P, S, L, K]
        { 0, 2, 1, 0, 1, 2 }, // 3: Scissors vs [R, P, S, L, K]
        { 0, 2, 1, 2, 0, 1 }, // 4: Lizard vs [R, P, S, L, K]
        { 0, 1, 2, 1, 2, 0 }  // 5: Spock vs [R, P, S, L, K]
    };

    // Action verb lookup when move A beats move B: [winner][loser]
    const char* const kVerbs[6][6] = {
        { "", "", "", "", "", "" },
        { "", "", "", "crushes", "crushes", "" },       // Rock beats Scissors, Lizard
        { "", "covers", "", "", "", "disproves" },      // Paper beats Rock, Spock
        { "", "", "cuts", "", "decapitates", "" },      // Scissors beats Paper, Lizard
        { "", "", "eats", "", "", "poisons" },          // Lizard beats Paper, Spock
        { "", "vaporizes", "", "smashes", "", "" }       // Spock beats Rock, Scissors
    };
}

bool RulesEngine::isLegalMove(Move move, RuleSet ruleSet) {
    if (move == Move::None) return false;
    if (ruleSet == RuleSet::Classic) {
        return (move >= Move::Rock && move <= Move::Scissors);
    }
    return (move >= Move::Rock && move <= Move::Spock);
}

std::vector<Move> RulesEngine::getLegalMoves(RuleSet ruleSet) {
    if (ruleSet == RuleSet::Classic) {
        return { Move::Rock, Move::Paper, Move::Scissors };
    }
    return { Move::Rock, Move::Paper, Move::Scissors, Move::Lizard, Move::Spock };
}

std::string RulesEngine::getVerb(Move winner, Move loser) {
    uint8_t w = static_cast<uint8_t>(winner);
    uint8_t l = static_cast<uint8_t>(loser);

    if (w <= 5 && l <= 5) {
        const char* v = kVerbs[w][l];
        if (v && v[0] != '\0') {
            return v;
        }
    }
    return "defeats";
}

Outcome RulesEngine::evaluate(Move p1, Move p2, RuleSet ruleSet) {
    if (p1 == p2) return Outcome::Tie;
    if (!isLegalMove(p1, ruleSet) || !isLegalMove(p2, ruleSet)) return Outcome::Tie;

    uint8_t a = static_cast<uint8_t>(p1);
    uint8_t b = static_cast<uint8_t>(p2);

    if (a > 5 || b > 5) return Outcome::Tie;

    uint8_t result = kRpslsMatrix[a][b];
    return static_cast<Outcome>(result);
}

std::string RulesEngine::getRoundDescription(Move p1, Move p2, Outcome outcome,
                                            const std::string& p1Name,
                                            const std::string& p2Name) {
    std::ostringstream oss;
    if (outcome == Outcome::Tie) {
        oss << "Draw! Both " << p1Name << " and " << p2Name << " threw " << InputValidator::moveToString(p1) << ".";
    } else if (outcome == Outcome::Player1Win) {
        std::string verb = getVerb(p1, p2);
        oss << p1Name << "'s " << InputValidator::moveToString(p1) << " " << verb << " " 
            << p2Name << "'s " << InputValidator::moveToString(p2) << "! " << p1Name << " takes the round!";
    } else {
        std::string verb = getVerb(p2, p1);
        oss << p2Name << "'s " << InputValidator::moveToString(p2) << " " << verb << " " 
            << p1Name << "'s " << InputValidator::moveToString(p1) << "! " << p2Name << " takes the round!";
    }
    return oss.str();
}

std::vector<Move> RulesEngine::getWinningCounters(Move targetMove, RuleSet ruleSet) {
    std::vector<Move> counters;
    for (Move candidate : getLegalMoves(ruleSet)) {
        if (evaluate(candidate, targetMove, ruleSet) == Outcome::Player1Win) {
            counters.push_back(candidate);
        }
    }
    return counters;
}

std::vector<Move> RulesEngine::getLosingVictims(Move myMove, RuleSet ruleSet) {
    std::vector<Move> victims;
    for (Move candidate : getLegalMoves(ruleSet)) {
        if (evaluate(myMove, candidate, ruleSet) == Outcome::Player1Win) {
            victims.push_back(candidate);
        }
    }
    return victims;
}

} // namespace rps
