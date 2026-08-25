#pragma once

#include "Types.hpp"
#include "StatsManager.hpp"
#include "HintSystem.hpp"
#include "AiGuesser.hpp"
#include "BullsAndCows.hpp"

namespace GuessGame {

    class GameEngine {
    public:
        GameEngine();

        void playClassic();
        void playSurvival();
        void playTimeAttack();
        void playReverseAi();
        void playDuel();
        void playBullsAndCows();

        Difficulty selectDifficulty(bool includeCustom = true);
        DifficultyConfig getDifficultyConfig(Difficulty diff, int customMin = 1, int customMax = 100);

        StatsManager& getStatsManager() noexcept { return m_stats; }
        const StatsManager& getStatsManager() const noexcept { return m_stats; }

    private:
        StatsManager m_stats;

        void handleGameOver(GameRoundSummary& summary);
        bool handleHintPrompt(int target, int minVal, int maxVal, std::vector<HintDetail>& hints, int& hintsUsed);
    };

} // namespace GuessGame
