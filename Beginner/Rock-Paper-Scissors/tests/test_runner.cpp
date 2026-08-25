#include "Types.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "RandomGenerator.hpp"
#include "RulesEngine.hpp"
#include "AiPredictor.hpp"
#include "GameTheoryAnalyzer.hpp"
#include "StatsManager.hpp"
#include "AsciiArt.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdio>

using namespace rps;

static int g_passedTests = 0;
static int g_failedTests = 0;

#define TEST_ASSERT(expr, msg) \
    do { \
        if (expr) { \
            std::cout << "    [PASS] " << msg << "\n"; \
            g_passedTests++; \
        } else { \
            std::cout << "    [FAIL] " << msg << " (" #expr ") at line " << __LINE__ << "\n"; \
            g_failedTests++; \
        } \
    } while(0)

void testClassicRules() {
    std::cout << "\n--- Suite 1: Classic 3-Way Rules ---\n";
    
    TEST_ASSERT(RulesEngine::evaluate(Move::Rock, Move::Scissors, RuleSet::Classic) == Outcome::Player1Win, "Rock beats Scissors");
    TEST_ASSERT(RulesEngine::evaluate(Move::Scissors, Move::Paper, RuleSet::Classic) == Outcome::Player1Win, "Scissors beats Paper");
    TEST_ASSERT(RulesEngine::evaluate(Move::Paper, Move::Rock, RuleSet::Classic) == Outcome::Player1Win, "Paper beats Rock");

    TEST_ASSERT(RulesEngine::evaluate(Move::Scissors, Move::Rock, RuleSet::Classic) == Outcome::Player2Win, "Scissors loses to Rock");
    TEST_ASSERT(RulesEngine::evaluate(Move::Paper, Move::Scissors, RuleSet::Classic) == Outcome::Player2Win, "Paper loses to Scissors");
    TEST_ASSERT(RulesEngine::evaluate(Move::Rock, Move::Paper, RuleSet::Classic) == Outcome::Player2Win, "Rock loses to Paper");

    TEST_ASSERT(RulesEngine::evaluate(Move::Rock, Move::Rock, RuleSet::Classic) == Outcome::Tie, "Rock ties Rock");
    TEST_ASSERT(RulesEngine::evaluate(Move::Paper, Move::Paper, RuleSet::Classic) == Outcome::Tie, "Paper ties Paper");
    TEST_ASSERT(RulesEngine::evaluate(Move::Scissors, Move::Scissors, RuleSet::Classic) == Outcome::Tie, "Scissors ties Scissors");
}

void testRpslsRules() {
    std::cout << "\n--- Suite 2: Extended 5-Way RPSLS Rules ---\n";
    
    TEST_ASSERT(RulesEngine::evaluate(Move::Scissors, Move::Paper, RuleSet::RPSLS) == Outcome::Player1Win, "Scissors cuts Paper");
    TEST_ASSERT(RulesEngine::evaluate(Move::Scissors, Move::Lizard, RuleSet::RPSLS) == Outcome::Player1Win, "Scissors decapitates Lizard");
    
    TEST_ASSERT(RulesEngine::evaluate(Move::Paper, Move::Rock, RuleSet::RPSLS) == Outcome::Player1Win, "Paper covers Rock");
    TEST_ASSERT(RulesEngine::evaluate(Move::Paper, Move::Spock, RuleSet::RPSLS) == Outcome::Player1Win, "Paper disproves Spock");

    TEST_ASSERT(RulesEngine::evaluate(Move::Rock, Move::Lizard, RuleSet::RPSLS) == Outcome::Player1Win, "Rock crushes Lizard");
    TEST_ASSERT(RulesEngine::evaluate(Move::Rock, Move::Scissors, RuleSet::RPSLS) == Outcome::Player1Win, "Rock crushes Scissors");

    TEST_ASSERT(RulesEngine::evaluate(Move::Lizard, Move::Spock, RuleSet::RPSLS) == Outcome::Player1Win, "Lizard poisons Spock");
    TEST_ASSERT(RulesEngine::evaluate(Move::Lizard, Move::Paper, RuleSet::RPSLS) == Outcome::Player1Win, "Lizard eats Paper");

    TEST_ASSERT(RulesEngine::evaluate(Move::Spock, Move::Scissors, RuleSet::RPSLS) == Outcome::Player1Win, "Spock smashes Scissors");
    TEST_ASSERT(RulesEngine::evaluate(Move::Spock, Move::Rock, RuleSet::RPSLS) == Outcome::Player1Win, "Spock vaporizes Rock");

    TEST_ASSERT(RulesEngine::evaluate(Move::Lizard, Move::Lizard, RuleSet::RPSLS) == Outcome::Tie, "Lizard ties Lizard");
    TEST_ASSERT(RulesEngine::evaluate(Move::Spock, Move::Spock, RuleSet::RPSLS) == Outcome::Tie, "Spock ties Spock");
}

void testActionVerbs() {
    std::cout << "\n--- Suite 3: Action Verbs ---\n";

    TEST_ASSERT(RulesEngine::getVerb(Move::Scissors, Move::Paper) == "cuts", "Scissors cuts Paper");
    TEST_ASSERT(RulesEngine::getVerb(Move::Paper, Move::Rock) == "covers", "Paper covers Rock");
    TEST_ASSERT(RulesEngine::getVerb(Move::Rock, Move::Lizard) == "crushes", "Rock crushes Lizard");
    TEST_ASSERT(RulesEngine::getVerb(Move::Lizard, Move::Spock) == "poisons", "Lizard poisons Spock");
    TEST_ASSERT(RulesEngine::getVerb(Move::Spock, Move::Scissors) == "smashes", "Spock smashes Scissors");
    TEST_ASSERT(RulesEngine::getVerb(Move::Scissors, Move::Lizard) == "decapitates", "Scissors decapitates Lizard");
    TEST_ASSERT(RulesEngine::getVerb(Move::Lizard, Move::Paper) == "eats", "Lizard eats Paper");
    TEST_ASSERT(RulesEngine::getVerb(Move::Paper, Move::Spock) == "disproves", "Paper disproves Spock");
    TEST_ASSERT(RulesEngine::getVerb(Move::Spock, Move::Rock) == "vaporizes", "Spock vaporizes Rock");
    TEST_ASSERT(RulesEngine::getVerb(Move::Rock, Move::Scissors) == "crushes", "Rock crushes Scissors");
}

void testCountersAndVictims() {
    std::cout << "\n--- Suite 4: Winning Counters & Losing Victims ---\n";

    auto classicRockCounters = RulesEngine::getWinningCounters(Move::Rock, RuleSet::Classic);
    TEST_ASSERT(classicRockCounters.size() == 1 && classicRockCounters[0] == Move::Paper, "Classic Rock counter is Paper");

    auto rpslsRockCounters = RulesEngine::getWinningCounters(Move::Rock, RuleSet::RPSLS);
    TEST_ASSERT(rpslsRockCounters.size() == 2, "RPSLS Rock has 2 winning counters");

    auto spockVictims = RulesEngine::getLosingVictims(Move::Spock, RuleSet::RPSLS);
    TEST_ASSERT(spockVictims.size() == 2, "Spock defeats 2 victims");
}

void testRandomGenerator() {
    std::cout << "\n--- Suite 5: Random Generator ---\n";

    RandomGenerator::setSeed(1337);
    bool classicValid = true;
    for (int i = 0; i < 500; ++i) {
        Move m = RandomGenerator::getRandomMove(RuleSet::Classic);
        if (m < Move::Rock || m > Move::Scissors) classicValid = false;
    }
    TEST_ASSERT(classicValid, "Classic random moves within valid bounds");

    bool rpslsValid = true;
    for (int i = 0; i < 500; ++i) {
        Move m = RandomGenerator::getRandomMove(RuleSet::RPSLS);
        if (m < Move::Rock || m > Move::Spock) rpslsValid = false;
    }
    TEST_ASSERT(rpslsValid, "RPSLS random moves within valid bounds");

    int intVal = RandomGenerator::getInt(10, 20);
    TEST_ASSERT(intVal >= 10 && intVal <= 20, "getInt range check");
}

void testShannonEntropy() {
    std::cout << "\n--- Suite 6: Shannon Entropy ---\n";

    double maxEnt3 = GameTheoryAnalyzer::calculateMaxEntropy(RuleSet::Classic);
    TEST_ASSERT(std::abs(maxEnt3 - std::log2(3.0)) < 0.0001, "Max entropy for Classic is log2(3)");

    std::vector<Move> constantMoves(50, Move::Rock);
    double zeroEnt = GameTheoryAnalyzer::calculateShannonEntropy(constantMoves, RuleSet::Classic);
    TEST_ASSERT(std::abs(zeroEnt) < 0.0001, "Constant sequence has 0 entropy");

    std::vector<Move> balancedMoves;
    for (int i = 0; i < 300; ++i) {
        balancedMoves.push_back(static_cast<Move>((i % 3) + 1));
    }
    double maxMeasured = GameTheoryAnalyzer::calculateShannonEntropy(balancedMoves, RuleSet::Classic);
    TEST_ASSERT(std::abs(maxMeasured - maxEnt3) < 0.0001, "Balanced sequence achieves maximum entropy");
}

void testNashEquilibrium() {
    std::cout << "\n--- Suite 7: Nash Equilibrium TVD ---\n";

    std::array<double, 6> uniformFreqs{};
    uniformFreqs[1] = 1.0 / 3.0;
    uniformFreqs[2] = 1.0 / 3.0;
    uniformFreqs[3] = 1.0 / 3.0;

    double distZero = GameTheoryAnalyzer::calculateNashEquilibriumDistance(uniformFreqs, RuleSet::Classic);
    TEST_ASSERT(std::abs(distZero) < 0.0001, "Uniform distribution has 0 Nash distance");

    std::array<double, 6> rockHeavy{};
    rockHeavy[1] = 1.0;
    double distHeavy = GameTheoryAnalyzer::calculateNashEquilibriumDistance(rockHeavy, RuleSet::Classic);
    TEST_ASSERT(distHeavy > 0.5, "Single move distribution has high Nash distance");
}

void testMarkovOracle() {
    std::cout << "\n--- Suite 8: Markov Prediction ---\n";

    MarkovOracleAi oracle;
    oracle.reset();

    // Repeating sequence: R -> P -> S -> R -> P...
    for (int i = 0; i < 20; ++i) {
        oracle.recordRound(Move::Rock, Move::Scissors, Outcome::Player1Win);
        oracle.recordRound(Move::Paper, Move::Rock, Outcome::Player1Win);
        oracle.recordRound(Move::Scissors, Move::Paper, Outcome::Player1Win);
    }

    Move predicted = oracle.predictMostLikelyPlayerMove(RuleSet::Classic);
    TEST_ASSERT(predicted == Move::Rock, "Markov Oracle detects repeating move transition");
}

void testWslsTactician() {
    std::cout << "\n--- Suite 9: WSLS Heuristics ---\n";

    WslsAi wsls;
    wsls.reset();

    wsls.recordRound(Move::Rock, Move::Scissors, Outcome::Player1Win);
    Move nextAiMove = wsls.selectMove(RuleSet::Classic);
    TEST_ASSERT(nextAiMove == Move::Paper, "WSLS counters repeated winning move");
}

void testStatsSerialization() {
    std::cout << "\n--- Suite 10: Profile Serialization ---\n";

    std::string tempFile = "test_rps_tmp.dat";
    std::remove(tempFile.c_str());

    {
        StatsManager sm(tempFile);
        sm.getProfile().playerName = "Tester";
        sm.getProfile().careerScore = 4500;
        sm.getProfile().totalWins = 35;
        sm.getProfile().rockCount = 20;
        sm.saveProfile();
    }

    {
        StatsManager sm2(tempFile);
        TEST_ASSERT(sm2.getProfile().playerName == "Tester", "Player name loaded correctly");
        TEST_ASSERT(sm2.getProfile().careerScore == 4500, "Career score loaded correctly");
        TEST_ASSERT(sm2.getProfile().totalWins == 35, "Total wins loaded correctly");
        TEST_ASSERT(sm2.getProfile().rockCount == 20, "Rock count loaded correctly");
    }

    std::remove(tempFile.c_str());
}

void testAchievementsAndRanks() {
    std::cout << "\n--- Suite 11: Achievements & Ranks ---\n";

    StatsManager sm("test_rps_ach.dat");
    sm.resetProfile();

    TEST_ASSERT(!sm.isAchievementUnlocked(AchievementId::FirstBlood), "First blood locked initially");
    sm.unlockAchievement(AchievementId::FirstBlood);
    TEST_ASSERT(sm.isAchievementUnlocked(AchievementId::FirstBlood), "First blood unlocked successfully");

    sm.getProfile().careerScore = 6000;
    auto rank = sm.getCurrentRank();
    TEST_ASSERT(rank.title == "Psychology Reader", "Mastery rank calculation check");

    std::remove("test_rps_ach.dat");
}

void testAsciiArtInvariants() {
    std::cout << "\n--- Suite 12: Visual & Layout Invariants ---\n";

    for (int m = 1; m <= 5; ++m) {
        Move move = static_cast<Move>(m);
        auto artR = AsciiArt::getHandArt(move, true);
        auto artL = AsciiArt::getHandArt(move, false);
        TEST_ASSERT(artR.size() == 6 && artL.size() == 6, "Hand art height consistency (6 lines)");
    }

    auto ranks = StatsManager::getAllRanks();
    TEST_ASSERT(ranks.size() == 6, "Total rank tiers count");
}

int main() {
    Terminal::init();

    std::cout << "\n======================================================\n";
    std::cout << "  ROCK-PAPER-SCISSORS UNIT TESTS                      \n";
    std::cout << "======================================================\n";

    testClassicRules();
    testRpslsRules();
    testActionVerbs();
    testCountersAndVictims();
    testRandomGenerator();
    testShannonEntropy();
    testNashEquilibrium();
    testMarkovOracle();
    testWslsTactician();
    testStatsSerialization();
    testAchievementsAndRanks();
    testAsciiArtInvariants();

    std::cout << "\n======================================================\n";
    std::cout << "  RESULTS: " << g_passedTests << " PASSED, " << g_failedTests << " FAILED\n";
    std::cout << "======================================================\n\n";

    return (g_failedTests == 0) ? 0 : 1;
}
