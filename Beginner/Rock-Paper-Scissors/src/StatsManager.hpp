#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace rps {

class StatsManager {
public:
    explicit StatsManager(const std::string& saveFilePath = "rps_stats.dat");

    bool loadProfile();
    bool saveProfile();
    void resetProfile();

    PlayerProfile& getProfile() { return m_profile; }
    const PlayerProfile& getProfile() const { return m_profile; }

    uint32_t calculateRoundScore(Outcome outcome, RuleSet ruleSet, AiPersonality aiLevel, uint32_t currentStreak);
    void recordRoundOutcome(Move playerMove, Move opponentMove, Outcome outcome, RuleSet ruleSet, AiPersonality aiLevel);
    void recordMatchOutcome(const MatchSummary& match);

    std::vector<Achievement> checkAchievements(const MatchSummary* completedMatch = nullptr);
    bool unlockAchievement(AchievementId id);
    bool isAchievementUnlocked(AchievementId id) const;

    MasteryRank getCurrentRank() const;
    static std::vector<MasteryRank> getAllRanks();

    void displayProfileCard() const;
    void displayAchievementsGrid() const;
    void displayLeaderboardRanks() const;

private:
    std::string m_saveFilePath;
    PlayerProfile m_profile;

    void initializeAchievements();
};

} // namespace rps
