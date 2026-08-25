#pragma once

#include "Types.hpp"
#include <vector>
#include <string>
#include <array>

namespace rps {

struct PlayerAnalyticsReport {
    uint32_t totalMoves{0};
    std::array<double, 6> moveFrequencies{};
    double shannonEntropy{0.0};
    double maxEntropy{0.0};
    double predictabilityScore{0.0};
    double nashDistance{0.0};
    double winStayRate{0.0};
    double loseShiftRate{0.0};
    std::string biasVerdict;
};

class GameTheoryAnalyzer {
public:
    static PlayerAnalyticsReport analyzeHistory(const std::vector<RoundResult>& history,
                                                RuleSet ruleSet,
                                                bool analyzePlayer1 = true);

    static double calculateShannonEntropy(const std::vector<Move>& moves, RuleSet ruleSet);
    static double calculateMaxEntropy(RuleSet ruleSet);
    static double calculateNashEquilibriumDistance(const std::array<double, 6>& freqs, RuleSet ruleSet);

    static void displayAnalyticsDashboard(const PlayerAnalyticsReport& report,
                                          RuleSet ruleSet,
                                          const std::string& playerName = "Player");
};

} // namespace rps
