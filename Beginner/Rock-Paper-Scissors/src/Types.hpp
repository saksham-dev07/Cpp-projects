#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace rps {

enum class Move : uint8_t {
    None = 0,
    Rock = 1,
    Paper = 2,
    Scissors = 3,
    Lizard = 4,
    Spock = 5
};

enum class RuleSet : uint8_t {
    Classic = 0, // Rock, Paper, Scissors
    RPSLS = 1    // Rock, Paper, Scissors, Lizard, Spock
};

enum class Outcome : uint8_t {
    Tie = 0,
    Player1Win = 1,
    Player2Win = 2
};

enum class AiPersonality : uint8_t {
    Random = 0,
    Brute = 1,
    Mimic = 2,
    TacticianWSLS = 3,
    MarkovOracle = 4
};

enum class GameMode : uint8_t {
    QuickMatch = 0,
    BestOfN = 1,
    Gauntlet = 2,
    Survival = 3,
    PassAndPlay = 4,
    AiSimulation = 5
};

enum class Theme : uint8_t {
    Cyberpunk = 0,
    Emerald = 1,
    Sunset = 2,
    Ocean = 3,
    Amethyst = 4,
    Monochrome = 5
};

struct RoundResult {
    uint32_t roundNumber{1};
    Move p1Move{Move::None};
    Move p2Move{Move::None};
    Outcome outcome{Outcome::Tie};
    std::string p1Name{"Player"};
    std::string p2Name{"AI"};
    std::string verb;
    std::string commentary;
    double durationSeconds{0.0};
};

struct MatchSummary {
    GameMode mode{GameMode::QuickMatch};
    RuleSet ruleSet{RuleSet::Classic};
    std::string p1Name;
    std::string p2Name;
    uint32_t p1Wins{0};
    uint32_t p2Wins{0};
    uint32_t ties{0};
    uint32_t totalRounds{0};
    uint32_t scoreEarned{0};
    bool p1OverallWinner{false};
    std::vector<RoundResult> history;
};

enum class AchievementId : uint8_t {
    FirstBlood = 0,
    ClassicMaster = 1,
    SpockLogic = 2,
    LizardKing = 3,
    FlawlessSweep = 4,
    GauntletConqueror = 5,
    SurvivalWarrior = 6,
    EntropyMaster = 7,
    MindReader = 8,
    CenturyFighter = 9,
    Tactician = 10,
    Grandmaster = 11
};

struct Achievement {
    AchievementId id;
    std::string code;
    std::string title;
    std::string description;
    bool unlocked{false};
    std::string unlockedDate;
};

struct PlayerProfile {
    std::string playerName{"Player 1"};
    uint32_t careerScore{0};
    uint32_t totalRoundsPlayed{0};
    uint32_t totalWins{0};
    uint32_t totalLosses{0};
    uint32_t totalTies{0};
    uint32_t currentStreak{0};
    uint32_t longestWinStreak{0};
    
    // Mode wins
    uint32_t quickMatchWins{0};
    uint32_t tournamentWins{0};
    uint32_t gauntletClears{0};
    uint32_t highestSurvivalRounds{0};

    // Move frequency tracking
    uint32_t rockCount{0};
    uint32_t paperCount{0};
    uint32_t scissorsCount{0};
    uint32_t lizardCount{0};
    uint32_t spockCount{0};

    std::map<AchievementId, Achievement> achievements;

    Theme activeTheme{Theme::Cyberpunk};
    bool soundEnabled{true};
    bool slowAnimations{true};
};

struct MasteryRank {
    std::string title;
    std::string stars;
    uint32_t minScore;
};

} // namespace rps
