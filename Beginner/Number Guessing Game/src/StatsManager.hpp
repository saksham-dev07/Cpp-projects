#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace GuessGame {

    class StatsManager {
    public:
        explicit StatsManager(const std::string& savePath = "stats.dat");

        PlayerProfile& getProfile() noexcept { return m_profile; }
        const PlayerProfile& getProfile() const noexcept { return m_profile; }

        bool loadProfile();
        bool saveProfile() const;
        void resetProfile();

        int calculateScore(const GameRoundSummary& summary) const;
        std::vector<AchievementId> processRoundResults(GameRoundSummary& summary);

        void addHighScore(const HighScoreEntry& entry);
        std::vector<HighScoreEntry> getTopHighScores(size_t limit = 10) const;

        std::vector<Achievement> getAllAchievements() const { return m_profile.achievements; }
        int getUnlockedAchievementCount() const;

    private:
        std::string m_savePath;
        PlayerProfile m_profile;

        void initializeDefaultAchievements();
        void checkAndUnlockAchievements(GameRoundSummary& summary, std::vector<AchievementId>& newUnlocks);
        std::string getCurrentDateTime() const;
    };

} // namespace GuessGame
