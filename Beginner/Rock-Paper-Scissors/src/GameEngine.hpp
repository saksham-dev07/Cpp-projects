#pragma once

#include "Types.hpp"
#include "StatsManager.hpp"
#include "AiPredictor.hpp"
#include <memory>
#include <vector>

namespace rps {

class GameEngine {
public:
    explicit GameEngine(StatsManager& statsManager);

    void playQuickMatch();
    void playBestOfN();
    void playGauntlet();
    void playSurvival();
    void playPassAndPlay();
    void playAiSimulation();

    RuleSet promptRuleSetSelection();
    AiPersonality promptAiPersonalitySelection();

private:
    StatsManager& m_statsManager;

    RoundResult executeRound(uint32_t roundNumber,
                             RuleSet ruleSet,
                             const std::string& p1Name,
                             const std::string& p2Name,
                             AiPredictor* aiPredictor = nullptr,
                             bool isPassAndPlay = false);

    void notifyAchievements(const std::vector<Achievement>& newlyUnlocked);
};

} // namespace rps
