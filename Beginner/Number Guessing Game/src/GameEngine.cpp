#include "GameEngine.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "RandomGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace GuessGame {

    GameEngine::GameEngine()
        : m_stats("stats.dat") {
    }

    Difficulty GameEngine::selectDifficulty(bool includeCustom) {
        Terminal::clearScreen();
        Terminal::printHeader("SELECT DIFFICULTY LEVEL", "Configure the number range and challenge parameters");

        const auto& c = Terminal::colors();
        std::cout << "  " << c.primary << "[1] " << c.accent << "Easy       " << c.muted << "-> Range: 1 to 50    | Max Attempts (Survival): 6" << c.reset << "\n";
        std::cout << "  " << c.primary << "[2] " << c.accent << "Medium     " << c.muted << "-> Range: 1 to 100   | Max Attempts (Survival): 7" << c.reset << "\n";
        std::cout << "  " << c.primary << "[3] " << c.accent << "Hard       " << c.muted << "-> Range: 1 to 500   | Max Attempts (Survival): 9" << c.reset << "\n";
        std::cout << "  " << c.primary << "[4] " << c.accent << "Extreme    " << c.muted << "-> Range: 1 to 1000  | Max Attempts (Survival): 10" << c.reset << "\n";
        if (includeCustom) {
            std::cout << "  " << c.primary << "[5] " << c.accent << "Custom     " << c.muted << "-> Set your own Min/Max bounds & Attempt limits" << c.reset << "\n";
        }
        std::cout << "\n";

        int maxChoice = includeCustom ? 5 : 4;
        int choice = InputValidator::getInt("Select difficulty option (1-" + std::to_string(maxChoice) + "):", 1, maxChoice);

        switch (choice) {
            case 1: return Difficulty::Easy;
            case 2: return Difficulty::Medium;
            case 3: return Difficulty::Hard;
            case 4: return Difficulty::Extreme;
            case 5: return Difficulty::Custom;
            default: return Difficulty::Medium;
        }
    }

    DifficultyConfig GameEngine::getDifficultyConfig(Difficulty diff, int customMin, int customMax) {
        DifficultyConfig cfg;
        switch (diff) {
            case Difficulty::Easy:
                cfg = { 1, 50, 6, 45, "Easy", 1 };
                break;
            case Difficulty::Medium:
                cfg = { 1, 100, 7, 60, "Medium", 2 };
                break;
            case Difficulty::Hard:
                cfg = { 1, 500, 9, 90, "Hard", 3 };
                break;
            case Difficulty::Extreme:
                cfg = { 1, 1000, 10, 120, "Extreme", 5 };
                break;
            case Difficulty::Custom:
                cfg.minVal = customMin;
                cfg.maxVal = customMax;
                cfg.maxAttempts = RandomGenerator::computeOptimalAttempts(customMin, customMax);
                cfg.timeLimitSeconds = 60;
                cfg.name = "Custom";
                cfg.scoreMultiplier = 2;
                break;
        }
        return cfg;
    }

    bool GameEngine::handleHintPrompt(int target, int minVal, int maxVal, std::vector<HintDetail>& hints, int& hintsUsed) {
        Terminal::clearScreen();
        Terminal::printHeader("TACTICAL HINT TERMINAL", "Purchase mathematical clues with in-game score");

        const auto& c = Terminal::colors();
        std::cout << "  " << c.muted << "Active Search Range: [" << minVal << " .. " << maxVal << "]\n\n" << c.reset;

        for (size_t i = 0; i < hints.size(); ++i) {
            std::cout << "  " << c.primary << "[" << (i + 1) << "] " << c.accent << Terminal::padRight(hints[i].name, 18) << c.reset;
            if (hints[i].revealed) {
                std::cout << c.success << "[REVEALED] " << hints[i].description << c.reset << "\n";
            } else {
                std::cout << c.warning << "[LOCKED] Cost: " << hints[i].cost << " pts" << c.reset << "\n";
            }
        }
        std::cout << "  " << c.muted << "[0] Return to Game" << c.reset << "\n\n";

        int choice = InputValidator::getInt("Choose a hint to reveal (0 to exit):", 0, static_cast<int>(hints.size()));
        if (choice == 0) return false;

        size_t idx = choice - 1;
        if (hints[idx].revealed) {
            std::cout << "\n  " << c.highlight << "[i] This hint is already revealed: " << hints[idx].description << c.reset << "\n";
        } else {
            hints[idx].revealed = true;
            hintsUsed++;
            Terminal::beep(m_stats.getProfile().soundEnabled);
            std::cout << "\n  " << c.success << "[★] Clue Revealed: " << hints[idx].description << c.reset << "\n";
        }
        Terminal::pausePrompt();
        return true;
    }

    void GameEngine::handleGameOver(GameRoundSummary& summary) {
        const auto& c = Terminal::colors();
        auto newUnlocks = m_stats.processRoundResults(summary);

        Terminal::clearScreen();
        if (summary.won) {
            Terminal::playFanfare(m_stats.getProfile().soundEnabled);
            std::cout << c.success << R"(
  ╔═════════════════════════════════════════════════════════════════════════╗
  ║    __     _____ ____ _____ ___  ______   __  _ _ _                      ║
  ║    \ \   / /_ _/ ___|_   _/ _ \|  _ \ \ / / | | | |                     ║
  ║     \ \ / / | | |     | || | | | |_) \ V /  | | | |                     ║
  ║      \ V /  | | |___  | || |_| |  _ < | |   |_|_|_|                     ║
  ║       \_/  |___\____| |_| \___/|_| \_\|_|   (_|_|_)                     ║
  ╚═════════════════════════════════════════════════════════════════════════╝
)" << c.reset;
            std::cout << "\n  " << c.highlight << "★ CONGRATULATIONS! You successfully identified the secret number " 
                      << c.accent << summary.targetNumber << c.highlight << "! ★" << c.reset << "\n\n";
        } else {
            Terminal::playErrorBeep(m_stats.getProfile().soundEnabled);
            std::cout << c.error << R"(
  ╔═════════════════════════════════════════════════════════════════════════╗
  ║       ____    _    __  __ _____    _____     _______ ____               ║
  ║      / ___|  / \  |  \/  | ____|  / _ \ \   / / ____|  _ \              ║
  ║     | |  _  / _ \ | |\/| |  _|   | | | \ \ / /|  _| | |_) |             ║
  ║     | |_| |/ ___ \| |  | | |___  | |_| |\ V / | |___|  _ <              ║
  ║      \____/_/   \_\_|  |_|_____|  \___/  \_/  |_____|_| \_\             ║
  ╚═════════════════════════════════════════════════════════════════════════╝
)" << c.reset;
            std::cout << "\n  " << c.error << "Mission Failed! The secret number was: " 
                      << c.accent << summary.targetNumber << c.reset << "\n\n";
        }

        // Summary Card
        std::vector<std::string> summaryLines = {
            "Mode: " + std::string(summary.mode == GameMode::Classic ? "Classic" :
                                   summary.mode == GameMode::Survival ? "Survival" :
                                   summary.mode == GameMode::TimeAttack ? "Time Attack" :
                                   summary.mode == GameMode::ReverseAI ? "Reverse AI" :
                                   summary.mode == GameMode::Duel ? "Duel Arena" : "Bulls & Cows"),
            "Difficulty: " + std::string(summary.difficulty == Difficulty::Easy ? "Easy" :
                                         summary.difficulty == Difficulty::Medium ? "Medium" :
                                         summary.difficulty == Difficulty::Hard ? "Hard" :
                                         summary.difficulty == Difficulty::Extreme ? "Extreme" : "Custom"),
            "Attempts Used: " + std::to_string(summary.attemptsUsed) + 
                (summary.maxAttemptsAllowed > 0 ? " / " + std::to_string(summary.maxAttemptsAllowed) : ""),
            "Time Elapsed: " + Terminal::formatTime(summary.elapsedSeconds),
            "Hints Used: " + std::to_string(summary.hintsUsed),
            "Score Earned: " + Terminal::formatScore(summary.scoreEarned),
            "Current Win Streak: " + std::to_string(m_stats.getProfile().currentStreak) + " wins",
            "Player Rank: " + m_stats.getProfile().getRankTitle()
        };
        Terminal::printBox(summaryLines, summary.won ? c.success : c.error);

        // Display Achievements Unlocked
        if (!newUnlocks.empty()) {
            std::cout << "\n  " << c.accent << "╔═══════════════════════════════════════════════════════════════════╗\n";
            std::cout << "  ║               ★ NEW ACHIEVEMENTS UNLOCKED! ★                     ║\n";
            std::cout << "  ╚═══════════════════════════════════════════════════════════════════╝" << c.reset << "\n";
            for (auto achId : newUnlocks) {
                for (const auto& ach : m_stats.getAllAchievements()) {
                    if (ach.id == achId) {
                        std::cout << "  " << c.success << "  [✓] " << ach.title << ": " << c.muted << ach.description << c.reset << "\n";
                    }
                }
            }
        }

        Terminal::pausePrompt();
    }

    void GameEngine::playClassic() {
        Difficulty diff = selectDifficulty(true);
        int customMin = 1, customMax = 100;
        if (diff == Difficulty::Custom) {
            Terminal::clearScreen();
            Terminal::printHeader("CUSTOM RANGE SETUP", "Define your lower and upper boundaries");
            customMin = InputValidator::getInt("Enter Minimum Bound (>= 1):", 1, 1000000);
            customMax = InputValidator::getInt("Enter Maximum Bound (>= Min):", customMin + 1, 1000000);
        }

        DifficultyConfig cfg = getDifficultyConfig(diff, customMin, customMax);
        int target = RandomGenerator::getInt(cfg.minVal, cfg.maxVal);
        int currentMin = cfg.minVal;
        int currentMax = cfg.maxVal;
        int attempts = 0;
        int hintsUsed = 0;
        std::vector<int> guesses;
        struct AttemptEntry {
            int guess;
            bool isLow;
            Proximity prox;
            int diff;
        };
        std::vector<AttemptEntry> attemptsHistory;
        auto hints = HintSystem::generateHintsForTarget(target, cfg.minVal, cfg.maxVal);

        auto startTime = std::chrono::steady_clock::now();

        while (true) {
            Terminal::clearScreen();
            Terminal::printHeader("CLASSIC NUMBER GUESSING ARENA", "Difficulty: " + cfg.name + " | Range: [" + std::to_string(cfg.minVal) + " - " + std::to_string(cfg.maxVal) + "]");

            const auto& c = Terminal::colors();
            std::cout << "  " << c.muted << "Active Search Interval : " << c.accent << "[" << currentMin << " .. " << currentMax << "]" << c.reset << "\n";
            std::cout << "  " << c.muted << "Total Guesses Made     : " << c.highlight << attempts << c.reset << "\n";
            std::cout << "  " << c.muted << "Hints Available        : " << c.warning << "Type -1 to open Tactical Hint Store" << c.reset << "\n";
            std::cout << "  " << c.muted << "Surrender Option       : " << c.error << "Type 0 to give up" << c.reset << "\n\n";

            if (!attemptsHistory.empty()) {
                const auto& last = attemptsHistory.back();
                std::vector<std::string> feedbackBox = {
                    "LATEST ATTEMPT #" + std::to_string(attempts) + " RESULT: [ " + std::to_string(last.guess) + " ]",
                    last.isLow ? "▲ TOO LOW! The secret number is HIGHER than " + std::to_string(last.guess) + "."
                               : "▼ TOO HIGH! The secret number is LOWER than " + std::to_string(last.guess) + "."
                };
                Terminal::printBox(feedbackBox, last.isLow ? c.warning : c.error);
                std::cout << "\n";
                Terminal::printProximityIndicator(last.prox, last.diff);
                std::cout << "\n";

                std::cout << "  " << c.muted << "Recent History: " << c.reset;
                for (size_t i = 0; i < attemptsHistory.size(); ++i) {
                    const auto& att = attemptsHistory[i];
                    std::cout << (att.isLow ? c.warning : c.error) << att.guess 
                              << (att.isLow ? " (▲ Low)" : " (▼ High)") << c.reset
                              << (i + 1 < attemptsHistory.size() ? c.muted + "  •  " : "");
                }
                std::cout << "\n\n";
            }

            int guess = InputValidator::getIntWithSpecial("Enter your guess [" + std::to_string(cfg.minVal) + ".." + std::to_string(cfg.maxVal) + "]:",
                                                         cfg.minVal, cfg.maxVal, -1, "Hint Menu (or 0 to give up)");

            if (guess == -1) {
                handleHintPrompt(target, currentMin, currentMax, hints, hintsUsed);
                continue;
            }

            if (guess == 0) {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = false;
                summary.targetNumber = target;
                summary.attemptsUsed = attempts;
                summary.elapsedSeconds = elapsed;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::Classic;
                summary.difficulty = diff;
                handleGameOver(summary);
                return;
            }

            attempts++;
            guesses.push_back(guess);
            Proximity prox = HintSystem::calculateProximity(guess, target, cfg.minVal, cfg.maxVal);
            int diffVal = std::abs(guess - target);

            if (guess == target) {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = true;
                summary.targetNumber = target;
                summary.attemptsUsed = attempts;
                summary.maxAttemptsAllowed = RandomGenerator::computeOptimalAttempts(cfg.minVal, cfg.maxVal);
                summary.elapsedSeconds = elapsed;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::Classic;
                summary.difficulty = diff;
                summary.guessHistory = guesses;
                handleGameOver(summary);
                return;
            } else if (guess < target) {
                currentMin = std::max(currentMin, guess + 1);
                attemptsHistory.push_back({guess, true, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            } else {
                currentMax = std::min(currentMax, guess - 1);
                attemptsHistory.push_back({guess, false, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            }
        }
    }

    void GameEngine::playSurvival() {
        Difficulty diff = selectDifficulty(false);
        DifficultyConfig cfg = getDifficultyConfig(diff);
        int target = RandomGenerator::getInt(cfg.minVal, cfg.maxVal);
        int currentMin = cfg.minVal;
        int currentMax = cfg.maxVal;
        int attempts = 0;
        int livesRemaining = cfg.maxAttempts;
        int hintsUsed = 0;
        std::vector<int> guesses;
        struct AttemptEntry {
            int guess;
            bool isLow;
            Proximity prox;
            int diff;
        };
        std::vector<AttemptEntry> attemptsHistory;
        auto hints = HintSystem::generateHintsForTarget(target, cfg.minVal, cfg.maxVal);

        auto startTime = std::chrono::steady_clock::now();

        while (livesRemaining > 0) {
            Terminal::clearScreen();
            Terminal::printHeader("SURVIVAL HARDCORE ARENA", "Difficulty: " + cfg.name + " | Range: [" + std::to_string(cfg.minVal) + " - " + std::to_string(cfg.maxVal) + "]");

            const auto& c = Terminal::colors();
            Terminal::printLivesBar(livesRemaining, cfg.maxAttempts);
            std::cout << "\n  " << c.muted << "Active Search Interval : " << c.accent << "[" << currentMin << " .. " << currentMax << "]" << c.reset << "\n";
            std::cout << "  " << c.muted << "Hints Available        : " << c.warning << "Type -1 for Hint Store" << c.reset << "\n\n";

            if (!attemptsHistory.empty()) {
                const auto& last = attemptsHistory.back();
                std::vector<std::string> feedbackBox = {
                    "LATEST ATTEMPT #" + std::to_string(attempts) + " RESULT: [ " + std::to_string(last.guess) + " ]",
                    last.isLow ? "▲ TOO LOW! Secret is HIGHER than " + std::to_string(last.guess) + ". (" + std::to_string(livesRemaining) + " shields left)"
                               : "▼ TOO HIGH! Secret is LOWER than " + std::to_string(last.guess) + ". (" + std::to_string(livesRemaining) + " shields left)"
                };
                Terminal::printBox(feedbackBox, last.isLow ? c.warning : c.error);
                std::cout << "\n";
                Terminal::printProximityIndicator(last.prox, last.diff);
                std::cout << "\n";

                std::cout << "  " << c.muted << "Recent History: " << c.reset;
                for (size_t i = 0; i < attemptsHistory.size(); ++i) {
                    const auto& att = attemptsHistory[i];
                    std::cout << (att.isLow ? c.warning : c.error) << att.guess 
                              << (att.isLow ? " (▲ Low)" : " (▼ High)") << c.reset
                              << (i + 1 < attemptsHistory.size() ? c.muted + "  •  " : "");
                }
                std::cout << "\n\n";
            }

            int guess = InputValidator::getIntWithSpecial("Enter your critical guess:", cfg.minVal, cfg.maxVal, -1, "Hint Menu");

            if (guess == -1) {
                handleHintPrompt(target, currentMin, currentMax, hints, hintsUsed);
                continue;
            }

            attempts++;
            livesRemaining--;
            guesses.push_back(guess);
            Proximity prox = HintSystem::calculateProximity(guess, target, cfg.minVal, cfg.maxVal);
            int diffVal = std::abs(guess - target);

            if (guess == target) {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = true;
                summary.targetNumber = target;
                summary.attemptsUsed = attempts;
                summary.maxAttemptsAllowed = cfg.maxAttempts;
                summary.elapsedSeconds = elapsed;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::Survival;
                summary.difficulty = diff;
                summary.guessHistory = guesses;
                handleGameOver(summary);
                return;
            } else if (guess < target) {
                currentMin = std::max(currentMin, guess + 1);
                attemptsHistory.push_back({guess, true, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            } else {
                currentMax = std::min(currentMax, guess - 1);
                attemptsHistory.push_back({guess, false, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            }
        }

        // Out of lives
        auto endTime = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        GameRoundSummary summary;
        summary.won = false;
        summary.targetNumber = target;
        summary.attemptsUsed = attempts;
        summary.maxAttemptsAllowed = cfg.maxAttempts;
        summary.elapsedSeconds = elapsed;
        summary.hintsUsed = hintsUsed;
        summary.mode = GameMode::Survival;
        summary.difficulty = diff;
        handleGameOver(summary);
    }

    void GameEngine::playTimeAttack() {
        Difficulty diff = selectDifficulty(false);
        DifficultyConfig cfg = getDifficultyConfig(diff);
        int target = RandomGenerator::getInt(cfg.minVal, cfg.maxVal);
        int currentMin = cfg.minVal;
        int currentMax = cfg.maxVal;
        int attempts = 0;
        int timeLimit = cfg.timeLimitSeconds;
        int hintsUsed = 0;
        std::vector<int> guesses;
        struct AttemptEntry {
            int guess;
            bool isLow;
            Proximity prox;
            int diff;
        };
        std::vector<AttemptEntry> attemptsHistory;
        auto hints = HintSystem::generateHintsForTarget(target, cfg.minVal, cfg.maxVal);

        auto startTime = std::chrono::steady_clock::now();

        while (true) {
            auto curTime = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(curTime - startTime).count();
            int remaining = static_cast<int>(timeLimit - elapsed);

            if (remaining <= 0) {
                GameRoundSummary summary;
                summary.won = false;
                summary.targetNumber = target;
                summary.attemptsUsed = attempts;
                summary.elapsedSeconds = timeLimit;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::TimeAttack;
                summary.difficulty = diff;
                handleGameOver(summary);
                return;
            }

            Terminal::clearScreen();
            Terminal::printHeader("TIME ATTACK SPEED RUN", "Difficulty: " + cfg.name + " | Time Limit: " + std::to_string(timeLimit) + "s");

            const auto& c = Terminal::colors();
            Terminal::printProgressBar(remaining, timeLimit, 35, "TIME REMAINING");
            std::cout << "\n  " << c.muted << "Active Search Interval : " << c.accent << "[" << currentMin << " .. " << currentMax << "]" << c.reset << "\n";
            std::cout << "  " << c.muted << "Attempts Made          : " << c.highlight << attempts << c.reset << "\n\n";

            if (!attemptsHistory.empty()) {
                const auto& last = attemptsHistory.back();
                std::vector<std::string> feedbackBox = {
                    "LATEST ATTEMPT #" + std::to_string(attempts) + " RESULT: [ " + std::to_string(last.guess) + " ]",
                    last.isLow ? "▲ TOO LOW! Secret is HIGHER than " + std::to_string(last.guess) + "!"
                               : "▼ TOO HIGH! Secret is LOWER than " + std::to_string(last.guess) + "!"
                };
                Terminal::printBox(feedbackBox, last.isLow ? c.warning : c.error);
                std::cout << "\n";
                Terminal::printProximityIndicator(last.prox, last.diff);
                std::cout << "\n";

                std::cout << "  " << c.muted << "Recent History: " << c.reset;
                for (size_t i = 0; i < attemptsHistory.size(); ++i) {
                    const auto& att = attemptsHistory[i];
                    std::cout << (att.isLow ? c.warning : c.error) << att.guess 
                              << (att.isLow ? " (▲ Low)" : " (▼ High)") << c.reset
                              << (i + 1 < attemptsHistory.size() ? c.muted + "  •  " : "");
                }
                std::cout << "\n\n";
            }

            int guess = InputValidator::getIntWithSpecial("Quickly enter your guess:", cfg.minVal, cfg.maxVal, -1, "Hint Menu");

            if (guess == -1) {
                handleHintPrompt(target, currentMin, currentMax, hints, hintsUsed);
                continue;
            }

            attempts++;
            guesses.push_back(guess);
            Proximity prox = HintSystem::calculateProximity(guess, target, cfg.minVal, cfg.maxVal);
            int diffVal = std::abs(guess - target);

            if (guess == target) {
                auto endTime = std::chrono::steady_clock::now();
                double finalElapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = true;
                summary.targetNumber = target;
                summary.attemptsUsed = attempts;
                summary.maxAttemptsAllowed = RandomGenerator::computeOptimalAttempts(cfg.minVal, cfg.maxVal);
                summary.elapsedSeconds = finalElapsed;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::TimeAttack;
                summary.difficulty = diff;
                summary.guessHistory = guesses;
                handleGameOver(summary);
                return;
            } else if (guess < target) {
                currentMin = std::max(currentMin, guess + 1);
                attemptsHistory.push_back({guess, true, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            } else {
                currentMax = std::min(currentMax, guess - 1);
                attemptsHistory.push_back({guess, false, prox, diffVal});
                Terminal::beep(m_stats.getProfile().soundEnabled);
            }
        }
    }

    void GameEngine::playReverseAi() {
        Terminal::clearScreen();
        Terminal::printHeader("REVERSE AI GUESSER", "Think of a number; watch AI solve it or catch you cheating!");

        const auto& c = Terminal::colors();
        std::cout << "  " << c.highlight << "Instructions:" << c.reset << "\n";
        std::cout << "  1. Think of an integer in a chosen range [1 .. N].\n";
        std::cout << "  2. The AI will make guesses based on your feedback.\n";
        std::cout << "  3. Enter " << c.accent << "'H'" << c.reset << " if your number is HIGHER than AI's guess.\n";
        std::cout << "  4. Enter " << c.accent << "'L'" << c.reset << " if your number is LOWER than AI's guess.\n";
        std::cout << "  5. Enter " << c.success << "'C'" << c.reset << " if AI guessed CORRECTLY!\n\n";

        std::cout << "  " << c.muted << "AI Search Strategy:" << c.reset << "\n";
        std::cout << "  " << c.primary << "[1] " << c.accent << "Binary Search AI      " << c.muted << "(Optimal, unbeatable log2 N solver)" << c.reset << "\n";
        std::cout << "  " << c.primary << "[2] " << c.accent << "Adaptive Heuristic AI " << c.muted << "(Semi-smart human emulation)" << c.reset << "\n";
        std::cout << "  " << c.primary << "[3] " << c.accent << "Random Guesser AI     " << c.muted << "(Narrowing uniform random)" << c.reset << "\n\n";

        int stratChoice = InputValidator::getInt("Select AI Strategy (1-3):", 1, 3);
        AiStrategy strategy = (stratChoice == 1) ? AiStrategy::BinarySearch :
                              (stratChoice == 2) ? AiStrategy::Heuristic : AiStrategy::NoviceRandom;

        int maxVal = InputValidator::getInt("Choose upper limit for your secret number (e.g., 100, 500, 1000):", 10, 1000000);
        AiGuesser ai(1, maxVal, strategy);

        int attempts = 0;
        auto startTime = std::chrono::steady_clock::now();

        Terminal::clearScreen();
        Terminal::printHeader("REVERSE AI GUESSER", "Range: [1 .. " + std::to_string(maxVal) + "]");
        std::cout << "  " << c.highlight << "[★] Keep your secret number firmly in mind! Press [Enter] when ready." << c.reset << "\n";
        Terminal::pausePrompt();

        while (true) {
            attempts++;
            int aiGuess = ai.makeGuess();

            Terminal::clearScreen();
            Terminal::printHeader("REVERSE AI GUESSER", "Strategy: " + std::string(strategy == AiStrategy::BinarySearch ? "Binary Search (Optimal)" :
                                                                                    strategy == AiStrategy::Heuristic ? "Adaptive Heuristic" : "Random Narrowing"));

            std::cout << "  " << c.muted << "AI Confidence Window : " << c.accent << "[" << ai.getLowBound() << " .. " << ai.getHighBound() << "]" << c.reset << "\n";
            std::cout << "  " << c.muted << "Possibilities Left   : " << c.highlight << ai.getRemainingPossibilities() << c.reset << "\n";
            std::cout << "  " << c.muted << "AI Attempt #         : " << c.primary << attempts << c.reset << "\n\n";

            std::vector<std::string> aiBox = {
                "AI Query #" + std::to_string(attempts) + ":",
                "Is your secret number: [ " + std::to_string(aiGuess) + " ] ?"
            };
            Terminal::printBox(aiBox, c.accent);

            std::cout << "\n  Feedback Options: " << c.accent << "[H] Higher  " << c.secondary << "[L] Lower  " << c.success << "[C] Correct" << c.reset << "\n";
            char fb = InputValidator::getChar("Your feedback for " + std::to_string(aiGuess) + ":", "HLC");

            if (fb == 'C') {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = true;
                summary.targetNumber = aiGuess;
                summary.attemptsUsed = attempts;
                summary.maxAttemptsAllowed = RandomGenerator::computeOptimalAttempts(1, maxVal);
                summary.elapsedSeconds = elapsed;
                summary.mode = GameMode::ReverseAI;
                summary.difficulty = Difficulty::Medium;

                Terminal::clearScreen();
                Terminal::playFanfare(m_stats.getProfile().soundEnabled);
                std::cout << "\n  " << c.success << "★ AI VICTORY! Found your number [" << aiGuess << "] in " << attempts << " attempts! ★" << c.reset << "\n\n";
                handleGameOver(summary);
                return;
            }

            bool valid = ai.processFeedback(aiGuess, fb);
            if (!valid || ai.hasContradiction()) {
                // Trigger Anti-Cheat
                Terminal::playErrorBeep(m_stats.getProfile().soundEnabled);
                Terminal::clearScreen();
                std::cout << c.error << R"(
  ╔═════════════════════════════════════════════════════════════════════════╗
  ║    _    _   _ _____ ___      ____ _   _ _____    _  _____               ║
  ║   / \  | \ | |_   _|_ _|    / ___| | | | ____|  / \|_   _|              ║
  ║  / _ \ |  \| | | |  | |    | |   | |_| |  _|   / _ \ | |                ║
  ║ / ___ \| |\  | | |  | |    | |___|  _  | |___ / ___ \| |                ║
  ║/_/   \_\_| \_| |_| |___|    \____|_| |_|_____/_/   \_\_|                ║
  ║                                                                         ║
  ║          ★ ANTI-CHEAT CONTRADICTION DETECTED! ★                        ║
  ╚═════════════════════════════════════════════════════════════════════════╝
)" << c.reset << "\n";

                std::vector<std::string> alarmLines = {
                    "ALARM: Inconsistent Player Clues Detected!",
                    ai.getContradictionExplanation(),
                    "The AI's search interval collapsed to empty space!"
                };
                Terminal::printBox(alarmLines, c.error);

                GameRoundSummary summary;
                summary.won = false;
                summary.contradictionCaught = true;
                summary.targetNumber = aiGuess;
                summary.attemptsUsed = attempts;
                summary.elapsedSeconds = 0.0;
                summary.mode = GameMode::ReverseAI;
                summary.difficulty = Difficulty::Medium;
                auto newUnlocks = m_stats.processRoundResults(summary);

                if (!newUnlocks.empty()) {
                    std::cout << "\n  " << c.accent << "╔═══════════════════════════════════════════════════════════════════╗\n";
                    std::cout << "  ║               ★ NEW ACHIEVEMENTS UNLOCKED! ★                     ║\n";
                    std::cout << "  ╚═══════════════════════════════════════════════════════════════════╝" << c.reset << "\n";
                    for (auto achId : newUnlocks) {
                        for (const auto& ach : m_stats.getAllAchievements()) {
                            if (ach.id == achId) {
                                std::cout << "  " << c.success << "  [✓] " << ach.title << ": " << c.muted << ach.description << c.reset << "\n";
                            }
                        }
                    }
                }

                Terminal::pausePrompt();
                return;
            }
        }
    }

    void GameEngine::playDuel() {
        Terminal::clearScreen();
        Terminal::printHeader("DUEL ARENA", "Turn-based multiplayer face-off");

        const auto& c = Terminal::colors();
        std::cout << "  " << c.primary << "[1] " << c.accent << "Player vs AI Race      " << c.muted << "-> Alternating turns to guess the secret number" << c.reset << "\n";
        std::cout << "  " << c.primary << "[2] " << c.accent << "Pass & Play (2 Players)" << c.muted << "-> Local 2-player showdown" << c.reset << "\n\n";

        int duelMode = InputValidator::getInt("Select Duel Mode (1-2):", 1, 2);

        Difficulty diff = selectDifficulty(false);
        DifficultyConfig cfg = getDifficultyConfig(diff);
        int target = RandomGenerator::getInt(cfg.minVal, cfg.maxVal);
        int currentMin = cfg.minVal;
        int currentMax = cfg.maxVal;

        std::string p1Name = m_stats.getProfile().username;
        std::string p2Name = (duelMode == 1) ? "AI Bot 9000" : "Player 2";
        if (duelMode == 2) {
            p2Name = InputValidator::getString("Enter Player 2 Name:");
        }

        AiGuesser ai(cfg.minVal, cfg.maxVal, AiStrategy::Heuristic);
        int turn = 1;
        int p1Attempts = 0;
        int p2Attempts = 0;
        std::string lastTurnMsg = "";
        bool lastTurnIsLow = false;

        auto startTime = std::chrono::steady_clock::now();

        while (true) {
            Terminal::clearScreen();
            std::string turnOwner = (turn % 2 == 1) ? p1Name : p2Name;
            Terminal::printHeader("DUEL ARENA: " + p1Name + " VS " + p2Name, "Current Turn: " + turnOwner + " | Range: [" + std::to_string(currentMin) + ".." + std::to_string(currentMax) + "]");

            std::cout << "  " << c.muted << "Round #" << ((turn + 1) / 2) << " | " 
                      << p1Name << " Guesses: " << p1Attempts << " | " << p2Name << " Guesses: " << p2Attempts << c.reset << "\n\n";

            if (!lastTurnMsg.empty()) {
                std::vector<std::string> turnCard = {
                    "PREVIOUS TURN ACTION:",
                    lastTurnMsg
                };
                Terminal::printBox(turnCard, lastTurnIsLow ? c.warning : c.error);
                std::cout << "\n";
            }

            int guess = 0;
            if (turn % 2 == 1) {
                // Player 1 turn
                p1Attempts++;
                guess = InputValidator::getInt("[" + p1Name + "] Enter your guess [" + std::to_string(currentMin) + ".." + std::to_string(currentMax) + "]:", currentMin, currentMax);
            } else {
                // Player 2 or AI turn
                p2Attempts++;
                if (duelMode == 1) {
                    std::cout << "  " << c.muted << "AI Bot is calculating trajectory..." << c.reset << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    guess = ai.makeGuess();
                    std::cout << "  " << c.secondary << "[" << p2Name << "] Guessed: " << c.accent << guess << c.reset << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                } else {
                    guess = InputValidator::getInt("[" + p2Name + "] Enter your guess [" + std::to_string(currentMin) + ".." + std::to_string(currentMax) + "]:", currentMin, currentMax);
                }
            }

            if (guess == target) {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();

                Terminal::clearScreen();
                Terminal::playFanfare(m_stats.getProfile().soundEnabled);
                std::cout << "\n  " << c.success << "★ VICTORY IN THE DUEL ARENA! ★" << c.reset << "\n";
                std::cout << "  " << c.accent << "Champion: " << turnOwner << " successfully guessed the secret " << target << "!" << c.reset << "\n\n";

                GameRoundSummary summary;
                summary.won = (turn % 2 == 1);
                summary.targetNumber = target;
                summary.attemptsUsed = p1Attempts;
                summary.elapsedSeconds = elapsed;
                summary.mode = GameMode::Duel;
                summary.difficulty = diff;
                handleGameOver(summary);
                return;
            } else if (guess < target) {
                currentMin = std::max(currentMin, guess + 1);
                ai.processFeedback(guess, 'H');
                Terminal::beep(m_stats.getProfile().soundEnabled);
                lastTurnIsLow = true;
                lastTurnMsg = "[" + turnOwner + "] guessed " + std::to_string(guess) + " -> ▲ TOO LOW! (Secret is HIGHER than " + std::to_string(guess) + ")";
            } else {
                currentMax = std::min(currentMax, guess - 1);
                ai.processFeedback(guess, 'L');
                Terminal::beep(m_stats.getProfile().soundEnabled);
                lastTurnIsLow = false;
                lastTurnMsg = "[" + turnOwner + "] guessed " + std::to_string(guess) + " -> ▼ TOO HIGH! (Secret is LOWER than " + std::to_string(guess) + ")";
            }

            turn++;
        }
    }

    void GameEngine::playBullsAndCows() {
        BullsAndCows game;
        game.reset();

        int attempts = 0;
        int maxAttempts = 10;
        int hintsUsed = 0;
        std::string guessedDigits = "";

        auto startTime = std::chrono::steady_clock::now();

        while (attempts < maxAttempts) {
            Terminal::clearScreen();
            Terminal::printHeader("BULLS & COWS (MASTERMIND 4-DIGIT)", "Crack the secret 4 distinct digits code");

            const auto& c = Terminal::colors();
            std::cout << "  " << c.muted << "Rules: " << c.accent << "Bulls" << c.muted << " = Correct digit in correct spot | " 
                      << c.secondary << "Cows" << c.muted << " = Correct digit in wrong spot\n";
            std::cout << "  " << c.muted << "Remaining Attempts: " << c.highlight << (maxAttempts - attempts) << "/" << maxAttempts << c.reset << "\n";
            std::cout << "  " << c.muted << "Type " << c.warning << "'HINT'" << c.muted << " to reveal a contained digit (costs score)" << c.reset << "\n\n";

            const auto& hist = game.getHistory();
            if (!hist.empty()) {
                std::vector<std::string> headers = {"Attempt #", "Guess", "Bulls (Exact)", "Cows (Mismatch)"};
                std::vector<std::vector<std::string>> rows;
                for (size_t i = 0; i < hist.size(); ++i) {
                    rows.push_back({
                        std::to_string(i + 1),
                        hist[i].guess,
                        std::to_string(hist[i].bulls) + " [Bull" + (hist[i].bulls == 1 ? "" : "s") + "]",
                        std::to_string(hist[i].cows) + " [Cow" + (hist[i].cows == 1 ? "" : "s") + "]"
                    });
                }
                Terminal::printTable(headers, rows);
                std::cout << "\n";
            }

            std::cout << c.accent << "Enter your 4-digit guess (or 'HINT'):" << c.reset << " ";
            std::string input;
            if (!std::getline(std::cin, input)) break;

            // Trim
            input.erase(input.begin(), std::find_if(input.begin(), input.end(), [](unsigned char ch) { return !std::isspace(ch); }));
            input.erase(std::find_if(input.rbegin(), input.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), input.end());

            std::string upperInput = input;
            std::transform(upperInput.begin(), upperInput.end(), upperInput.begin(), ::toupper);

            if (upperInput == "HINT") {
                char hintDigit = game.revealRandomContainedDigit(guessedDigits);
                guessedDigits += hintDigit;
                hintsUsed++;
                std::cout << "  " << c.warning << "[TACTICAL HINT] The secret code contains digit: '" << hintDigit << "'" << c.reset << "\n";
                Terminal::pausePrompt();
                continue;
            }

            if (!BullsAndCows::isValidCode(input)) {
                std::cout << "  " << c.error << "[!] Invalid format. Must be 4 UNIQUE digits (e.g. 3841)." << c.reset << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }

            attempts++;
            BullsCowsResult res = game.submitGuess(input);

            if (res.isWon()) {
                auto endTime = std::chrono::steady_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                GameRoundSummary summary;
                summary.won = true;
                summary.targetNumber = std::stoi(game.getSecret());
                summary.attemptsUsed = attempts;
                summary.maxAttemptsAllowed = maxAttempts;
                summary.elapsedSeconds = elapsed;
                summary.hintsUsed = hintsUsed;
                summary.mode = GameMode::BullsAndCows;
                summary.difficulty = Difficulty::Hard;
                handleGameOver(summary);
                return;
            }
        }

        // Out of attempts
        auto endTime = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        GameRoundSummary summary;
        summary.won = false;
        summary.targetNumber = std::stoi(game.getSecret());
        summary.attemptsUsed = attempts;
        summary.maxAttemptsAllowed = maxAttempts;
        summary.elapsedSeconds = elapsed;
        summary.hintsUsed = hintsUsed;
        summary.mode = GameMode::BullsAndCows;
        summary.difficulty = Difficulty::Hard;
        handleGameOver(summary);
    }

} // namespace GuessGame
