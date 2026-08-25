#include "Types.hpp"
#include "RandomGenerator.hpp"
#include "HintSystem.hpp"
#include "AiGuesser.hpp"
#include "BullsAndCows.hpp"
#include "StatsManager.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

using namespace GuessGame;

// Simple test framework
static int g_testsRun = 0;
static int g_testsPassed = 0;

#define TEST(name) void name()
#define RUN_TEST(name) do { \
    g_testsRun++; \
    std::cout << "[RUNNING] " << #name << "... "; \
    try { \
        name(); \
        g_testsPassed++; \
        std::cout << "\033[32mPASS\033[0m\n"; \
    } catch (const std::exception& e) { \
        std::cout << "\033[31mFAIL (Exception: " << e.what() << ")\033[0m\n"; \
    } catch (...) { \
        std::cout << "\033[31mFAIL (Unknown exception)\033[0m\n"; \
    } \
} while(0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        std::cerr << "\n  [ASSERT FAILED] " #expr " at line " << __LINE__ << "\n"; \
        throw std::runtime_error("Assertion failed: " #expr); \
    } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        std::cerr << "\n  [ASSERT FAILED] " #a " == " #b " (" << (a) << " != " << (b) << ") at line " << __LINE__ << "\n"; \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    } \
} while(0)

// ==========================================
// Test Cases
// ==========================================

TEST(Test_RandomGenerator_Bounds) {
    for (int i = 0; i < 100; ++i) {
        int r = RandomGenerator::getInt(10, 20);
        ASSERT_TRUE(r >= 10 && r <= 20);
    }
}

TEST(Test_RandomGenerator_OptimalAttempts) {
    ASSERT_EQ(RandomGenerator::computeOptimalAttempts(1, 1), 1);
    ASSERT_EQ(RandomGenerator::computeOptimalAttempts(1, 50), 6);    // ceil(log2(50)) = 6
    ASSERT_EQ(RandomGenerator::computeOptimalAttempts(1, 100), 7);   // ceil(log2(100)) = 7
    ASSERT_EQ(RandomGenerator::computeOptimalAttempts(1, 500), 9);   // ceil(log2(500)) = 9
    ASSERT_EQ(RandomGenerator::computeOptimalAttempts(1, 1000), 10); // ceil(log2(1000)) = 10
}

TEST(Test_HintSystem_PrimeCheck) {
    ASSERT_TRUE(!HintSystem::isPrime(0));
    ASSERT_TRUE(!HintSystem::isPrime(1));
    ASSERT_TRUE(HintSystem::isPrime(2));
    ASSERT_TRUE(HintSystem::isPrime(3));
    ASSERT_TRUE(!HintSystem::isPrime(4));
    ASSERT_TRUE(HintSystem::isPrime(5));
    ASSERT_TRUE(HintSystem::isPrime(97));
    ASSERT_TRUE(!HintSystem::isPrime(100));
}

TEST(Test_HintSystem_FactorsAndDigits) {
    auto f12 = HintSystem::getFactors(12);
    std::vector<int> expected12 = {1, 2, 3, 4, 6, 12};
    ASSERT_EQ(f12.size(), expected12.size());
    for (size_t i = 0; i < f12.size(); ++i) {
        ASSERT_EQ(f12[i], expected12[i]);
    }

    ASSERT_EQ(HintSystem::getDigitSum(1234), 10);
    ASSERT_EQ(HintSystem::getDigitSum(0), 0);
    ASSERT_EQ(HintSystem::getDigitSum(999), 27);

    ASSERT_EQ(HintSystem::getDigitCount(0), 1);
    ASSERT_EQ(HintSystem::getDigitCount(7), 1);
    ASSERT_EQ(HintSystem::getDigitCount(54321), 5);
}

TEST(Test_HintSystem_ProximityRadar) {
    ASSERT_TRUE(HintSystem::calculateProximity(50, 50, 1, 100) == Proximity::Boiling);
    ASSERT_TRUE(HintSystem::calculateProximity(52, 50, 1, 100) == Proximity::Boiling);
    ASSERT_TRUE(HintSystem::calculateProximity(55, 50, 1, 100) == Proximity::Burning);
    ASSERT_TRUE(HintSystem::calculateProximity(70, 50, 1, 100) == Proximity::Warm);
    ASSERT_TRUE(HintSystem::calculateProximity(90, 50, 1, 100) == Proximity::Cold);
    ASSERT_TRUE(HintSystem::calculateProximity(100, 10, 1, 100) == Proximity::Freezing);
}

TEST(Test_HintSystem_NarrowedBounds) {
    int target = 65;
    auto [minB, maxB] = HintSystem::getNarrowedBounds(target, 1, 100, 0.40);
    ASSERT_TRUE(target >= minB && target <= maxB);
    ASSERT_TRUE(minB >= 1);
    ASSERT_TRUE(maxB <= 100);
    ASSERT_TRUE((maxB - minB) <= 50);
}

TEST(Test_AiGuesser_BinarySearchConvergence) {
    int target = 73;
    AiGuesser ai(1, 100, AiStrategy::BinarySearch);

    int attempts = 0;
    while (attempts < 15) {
        attempts++;
        int guess = ai.makeGuess();
        if (guess == target) {
            ASSERT_TRUE(ai.processFeedback(guess, 'C'));
            break;
        } else if (guess < target) {
            ASSERT_TRUE(ai.processFeedback(guess, 'H'));
        } else {
            ASSERT_TRUE(ai.processFeedback(guess, 'L'));
        }
    }

    // Binary search in [1..100] takes <= 7 attempts
    ASSERT_TRUE(attempts <= 7);
    ASSERT_TRUE(!ai.hasContradiction());
}

TEST(Test_AiGuesser_AntiCheatDetection) {
    AiGuesser ai(1, 100, AiStrategy::BinarySearch);

    // AI guesses 50 -> Player says HIGHER (range is now 51..100)
    ASSERT_TRUE(ai.processFeedback(50, 'H'));
    ASSERT_EQ(ai.getLowBound(), 51);

    // AI guesses 75 -> Player says LOWER (range is now 51..74)
    ASSERT_TRUE(ai.processFeedback(75, 'L'));
    ASSERT_EQ(ai.getHighBound(), 74);

    // Player now claims number is LOWER than 51 -> contradiction!
    bool valid = ai.processFeedback(51, 'L');
    ASSERT_TRUE(!valid);
    ASSERT_TRUE(ai.hasContradiction());
    ASSERT_TRUE(!ai.getContradictionExplanation().empty());
}

TEST(Test_BullsAndCows_Validation) {
    ASSERT_TRUE(BullsAndCows::isValidCode("1234"));
    ASSERT_TRUE(BullsAndCows::isValidCode("0987"));
    ASSERT_TRUE(!BullsAndCows::isValidCode("1123")); // Duplicate '1'
    ASSERT_TRUE(!BullsAndCows::isValidCode("123"));  // 3 digits
    ASSERT_TRUE(!BullsAndCows::isValidCode("12345"));// 5 digits
    ASSERT_TRUE(!BullsAndCows::isValidCode("12a4")); // Non-digit
}

TEST(Test_BullsAndCows_Evaluation) {
    std::string secret = "1234";

    // All correct
    auto r1 = BullsAndCows::evaluate("1234", secret);
    ASSERT_EQ(r1.bulls, 4);
    ASSERT_EQ(r1.cows, 0);
    ASSERT_TRUE(r1.isWon());

    // All mismatch position
    auto r2 = BullsAndCows::evaluate("4321", secret);
    ASSERT_EQ(r2.bulls, 0);
    ASSERT_EQ(r2.cows, 4);
    ASSERT_TRUE(!r2.isWon());

    // Partial matches
    auto r3 = BullsAndCows::evaluate("1325", secret); // '1' bull, '3' cow, '2' cow, '5' none
    ASSERT_EQ(r3.bulls, 1);
    ASSERT_EQ(r3.cows, 2);

    // None correct
    auto r4 = BullsAndCows::evaluate("5678", secret);
    ASSERT_EQ(r4.bulls, 0);
    ASSERT_EQ(r4.cows, 0);
}

TEST(Test_StatsManager_SaveLoadCycle) {
    std::string testFile = "test_stats_temp.dat";
    {
        StatsManager sm(testFile);
        sm.getProfile().username = "CyberKnight";
        sm.getProfile().totalWins = 15;
        sm.getProfile().currentStreak = 4;
        sm.getProfile().totalScore = 4250;
        ASSERT_TRUE(sm.saveProfile());
    }

    {
        StatsManager sm2(testFile);
        ASSERT_EQ(sm2.getProfile().username, "CyberKnight");
        ASSERT_EQ(sm2.getProfile().totalWins, 15);
        ASSERT_EQ(sm2.getProfile().currentStreak, 4);
        ASSERT_EQ(sm2.getProfile().totalScore, 4250);
    }

    // Clean up temp file
    std::remove(testFile.c_str());
}

TEST(Test_StatsManager_ScoreCalculation) {
    StatsManager sm("test_score_temp.dat");
    GameRoundSummary summary;
    summary.won = true;
    summary.difficulty = Difficulty::Medium;
    summary.mode = GameMode::Classic;
    summary.attemptsUsed = 5;
    int score = sm.calculateScore(summary);
    ASSERT_TRUE(score > 300);

    std::remove("test_score_temp.dat");
}

TEST(Test_StatsManager_AntiCheatAchievement) {
    std::string testFile = "test_anticheat_temp.dat";
    StatsManager sm(testFile);
    GameRoundSummary summary;
    summary.won = false;
    summary.contradictionCaught = true;
    summary.mode = GameMode::ReverseAI;
    summary.difficulty = Difficulty::Medium;
    summary.attemptsUsed = 3;

    auto unlocks = sm.processRoundResults(summary);
    bool found = false;
    for (auto id : unlocks) {
        if (id == AchievementId::AntiCheatSleuth) found = true;
    }
    ASSERT_TRUE(found);

    std::remove(testFile.c_str());
}

int main() {
    std::cout << "\n=======================================================\n";
    std::cout << "  NUMBER GUESSING GAME - UNIT TEST RUNNER (C++17)      \n";
    std::cout << "=======================================================\n\n";

    RUN_TEST(Test_RandomGenerator_Bounds);
    RUN_TEST(Test_RandomGenerator_OptimalAttempts);
    RUN_TEST(Test_HintSystem_PrimeCheck);
    RUN_TEST(Test_HintSystem_FactorsAndDigits);
    RUN_TEST(Test_HintSystem_ProximityRadar);
    RUN_TEST(Test_HintSystem_NarrowedBounds);
    RUN_TEST(Test_AiGuesser_BinarySearchConvergence);
    RUN_TEST(Test_AiGuesser_AntiCheatDetection);
    RUN_TEST(Test_BullsAndCows_Validation);
    RUN_TEST(Test_BullsAndCows_Evaluation);
    RUN_TEST(Test_StatsManager_SaveLoadCycle);
    RUN_TEST(Test_StatsManager_ScoreCalculation);
    RUN_TEST(Test_StatsManager_AntiCheatAchievement);

    std::cout << "\n-------------------------------------------------------\n";
    std::cout << "Test Summary: " << g_testsPassed << "/" << g_testsRun << " passed.\n";
    if (g_testsPassed == g_testsRun) {
        std::cout << "\033[32m[SUCCESS] ALL UNIT TESTS COMPLETED SUCCESSFULLY!\033[0m\n";
        return 0;
    } else {
        std::cout << "\033[31m[FAILURE] SOME TESTS FAILED!\033[0m\n";
        return 1;
    }
}
