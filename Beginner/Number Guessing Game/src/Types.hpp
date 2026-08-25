#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

namespace GuessGame {

    enum class GameMode {
        Classic,
        Survival,
        TimeAttack,
        ReverseAI,
        Duel,
        BullsAndCows
    };

    enum class Difficulty {
        Easy,       // 1 - 50
        Medium,     // 1 - 100
        Hard,       // 1 - 500
        Extreme,    // 1 - 1000
        Custom
    };

    enum class Proximity {
        Boiling,
        Burning,
        Hot,
        Warm,
        Cold,
        Freezing
    };

    enum class AiStrategy {
        BinarySearch,
        Heuristic,
        NoviceRandom
    };

    enum class DuelOpponent {
        PlayerVsAI,
        PlayerVsPlayer
    };

    enum class ThemeColor {
        NeonCyberpunk,
        RetroEmerald,
        SunsetAmber,
        OceanBlue,
        Monochrome
    };

    enum class AchievementId {
        FirstWin,
        LuckyOne,
        BinaryProdigy,
        SpeedDemon,
        StreakTitan,
        SurvivalMaster,
        AntiCheatSleuth,
        MastermindAce,
        MathScholar,
        CenturyClub,
        FlawlessDuel,
        Grandmaster
    };

    struct DifficultyConfig {
        int minVal = 1;
        int maxVal = 100;
        int maxAttempts = 0;
        int timeLimitSeconds = 0;
        std::string name;
        int scoreMultiplier = 1;
    };

    struct Hint {
        std::string description;
        int cost = 0;
        bool used = false;
    };

    struct BullsCowsResult {
        int bulls = 0;
        int cows = 0;
        bool isWon() const noexcept { return bulls == 4; }
    };

    struct Achievement {
        AchievementId id;
        std::string title;
        std::string description;
        std::string icon;
        bool unlocked = false;
        std::string unlockedAt;
    };

    struct HighScoreEntry {
        std::string playerName;
        GameMode mode = GameMode::Classic;
        Difficulty difficulty = Difficulty::Medium;
        int score = 0;
        int attempts = 0;
        double timeSeconds = 0.0;
        std::string date;
    };

    struct PlayerProfile {
        std::string username = "Player";
        int totalGamesPlayed = 0;
        int totalWins = 0;
        int currentStreak = 0;
        int maxStreak = 0;
        int totalScore = 0;
        double totalTimeSeconds = 0.0;
        int hintsUsedCount = 0;
        std::vector<Achievement> achievements;
        std::vector<HighScoreEntry> highScores;
        ThemeColor activeTheme = ThemeColor::NeonCyberpunk;
        bool soundEnabled = true;

        double getWinRate() const noexcept {
            if (totalGamesPlayed == 0) return 0.0;
            return (static_cast<double>(totalWins) / totalGamesPlayed) * 100.0;
        }

        std::string getRankTitle() const {
            if (totalScore >= 15000) return "Grandmaster Oracle (***)";
            if (totalScore >= 10000) return "Master Mind Reader (**)";
            if (totalScore >= 5000)  return "Binary Seeker (*)";
            if (totalScore >= 2000)  return "Adept Guesser";
            if (totalScore >= 500)   return "Apprentice";
            return "Novice Guesser";
        }
    };

    struct GameRoundSummary {
        bool won = false;
        bool contradictionCaught = false;
        int targetNumber = 0;
        int attemptsUsed = 0;
        int maxAttemptsAllowed = 0;
        double elapsedSeconds = 0.0;
        int scoreEarned = 0;
        int hintsUsed = 0;
        GameMode mode = GameMode::Classic;
        Difficulty difficulty = Difficulty::Medium;
        std::vector<int> guessHistory;
        std::vector<std::string> feedbackHistory;
        std::vector<AchievementId> newlyUnlockedAchievements;
    };

} // namespace GuessGame
