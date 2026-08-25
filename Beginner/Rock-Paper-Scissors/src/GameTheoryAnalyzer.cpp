#include "GameTheoryAnalyzer.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace rps {

double GameTheoryAnalyzer::calculateMaxEntropy(RuleSet ruleSet) {
    double k = (ruleSet == RuleSet::Classic) ? 3.0 : 5.0;
    return std::log2(k);
}

double GameTheoryAnalyzer::calculateShannonEntropy(const std::vector<Move>& moves, RuleSet ruleSet) {
    if (moves.empty()) return 0.0;

    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;
    std::array<uint32_t, 6> counts{};
    counts.fill(0);

    for (Move m : moves) {
        uint8_t idx = static_cast<uint8_t>(m);
        if (idx >= 1 && idx <= maxIdx) {
            counts[idx]++;
        }
    }

    double total = static_cast<double>(moves.size());
    double entropy = 0.0;

    for (int i = 1; i <= maxIdx; ++i) {
        if (counts[i] > 0) {
            double p = static_cast<double>(counts[i]) / total;
            entropy -= p * std::log2(p);
        }
    }

    return entropy;
}

double GameTheoryAnalyzer::calculateNashEquilibriumDistance(const std::array<double, 6>& freqs, RuleSet ruleSet) {
    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;
    double uniformP = 1.0 / static_cast<double>(maxIdx);
    double tvd = 0.0;

    for (int i = 1; i <= maxIdx; ++i) {
        tvd += std::abs(freqs[i] - uniformP);
    }
    return 0.5 * tvd; // Total Variation Distance
}

PlayerAnalyticsReport GameTheoryAnalyzer::analyzeHistory(const std::vector<RoundResult>& history,
                                                        RuleSet ruleSet,
                                                        bool analyzePlayer1) {
    PlayerAnalyticsReport report;
    report.totalMoves = static_cast<uint32_t>(history.size());
    report.maxEntropy = calculateMaxEntropy(ruleSet);

    if (history.empty()) {
        report.biasVerdict = "Not enough rounds played yet to determine playstyle.";
        return report;
    }

    std::vector<Move> moves;
    moves.reserve(history.size());

    std::array<uint32_t, 6> counts{};
    counts.fill(0);

    uint32_t winOpportunities = 0;
    uint32_t winStayCount = 0;
    uint32_t lossOpportunities = 0;
    uint32_t lossShiftCount = 0;

    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;

    for (size_t i = 0; i < history.size(); ++i) {
        const auto& round = history[i];
        Move m = analyzePlayer1 ? round.p1Move : round.p2Move;
        moves.push_back(m);

        uint8_t idx = static_cast<uint8_t>(m);
        if (idx >= 1 && idx <= maxIdx) {
            counts[idx]++;
        }

        if (i > 0) {
            const auto& prev = history[i - 1];
            Outcome prevOutcome = prev.outcome;
            Move prevMove = analyzePlayer1 ? prev.p1Move : prev.p2Move;

            bool wonPrev = (analyzePlayer1 && prevOutcome == Outcome::Player1Win) ||
                           (!analyzePlayer1 && prevOutcome == Outcome::Player2Win);
            bool lostPrev = (analyzePlayer1 && prevOutcome == Outcome::Player2Win) ||
                            (!analyzePlayer1 && prevOutcome == Outcome::Player1Win);

            if (wonPrev) {
                winOpportunities++;
                if (m == prevMove) winStayCount++;
            } else if (lostPrev) {
                lossOpportunities++;
                if (m != prevMove) lossShiftCount++;
            }
        }
    }

    double total = static_cast<double>(moves.size());
    for (int i = 1; i <= maxIdx; ++i) {
        report.moveFrequencies[i] = static_cast<double>(counts[i]) / total;
    }

    report.shannonEntropy = calculateShannonEntropy(moves, ruleSet);
    if (report.maxEntropy > 0.0) {
        report.predictabilityScore = std::max(0.0, std::min(1.0, 1.0 - (report.shannonEntropy / report.maxEntropy)));
    }
    report.nashDistance = calculateNashEquilibriumDistance(report.moveFrequencies, ruleSet);

    report.winStayRate = (winOpportunities > 0) ? (static_cast<double>(winStayCount) / winOpportunities) : 0.50;
    report.loseShiftRate = (lossOpportunities > 0) ? (static_cast<double>(lossShiftCount) / lossOpportunities) : 0.50;

    if (report.totalMoves < 5) {
        report.biasVerdict = "Gathering more round data...";
    } else if (report.predictabilityScore < 0.12 && report.nashDistance < 0.12) {
        report.biasVerdict = "Unpredictable / Nash-Balanced (hard to counter)";
    } else if (report.winStayRate > 0.65 && report.loseShiftRate > 0.65) {
        report.biasVerdict = "Strong Win-Stay / Lose-Shift pattern";
    } else if (report.moveFrequencies[1] > 0.45) {
        report.biasVerdict = "Rock-heavy preference";
    } else if (report.moveFrequencies[2] > 0.45) {
        report.biasVerdict = "Paper-heavy preference";
    } else if (report.moveFrequencies[3] > 0.45) {
        report.biasVerdict = "Scissors-heavy preference";
    } else if (ruleSet == RuleSet::RPSLS && (report.moveFrequencies[4] > 0.35 || report.moveFrequencies[5] > 0.35)) {
        report.biasVerdict = "Lizard/Spock specialist";
    } else if (report.predictabilityScore > 0.40) {
        report.biasVerdict = "High repetition / predictable sequences";
    } else {
        report.biasVerdict = "Balanced tactical play";
    }

    return report;
}

void GameTheoryAnalyzer::displayAnalyticsDashboard(const PlayerAnalyticsReport& report,
                                                  RuleSet ruleSet,
                                                  const std::string& playerName) {
    Terminal::printHeader("GAME THEORY & PLAYSTYLE REPORT", playerName);

    int maxIdx = (ruleSet == RuleSet::Classic) ? 3 : 5;

    std::cout << "  " << Terminal::primary("Player: ") << Terminal::bold(playerName) 
              << " | " << Terminal::muted("Rounds Evaluated: ") << report.totalMoves << "\n\n";

    std::cout << "  " << Terminal::secondary("Move Frequencies:") << "\n";
    for (int i = 1; i <= maxIdx; ++i) {
        Move m = static_cast<Move>(i);
        std::string moveName = InputValidator::moveToStringWithIcon(m);
        double p = report.moveFrequencies[i];
        int percent = static_cast<int>(std::round(p * 100.0));

        std::ostringstream oss;
        oss << std::left << std::setw(15) << moveName << " [";
        int barLength = 24;
        int filled = static_cast<int>(p * barLength);
        oss << std::string(filled, '#') << std::string(barLength - filled, '.');
        oss << "] " << std::right << std::setw(3) << percent << "%";

        std::cout << "    " << oss.str() << "\n";
    }

    std::cout << "\n  " << Terminal::secondary("Information Theory & Strategy Metrics:") << "\n";

    std::ostringstream entOss;
    entOss << std::fixed << std::setprecision(2) << report.shannonEntropy << " bits (max: " 
           << std::fixed << std::setprecision(2) << report.maxEntropy << " bits)";
    std::cout << "    - Shannon Entropy H(X):  " << Terminal::accent(entOss.str()) << "\n";

    int predPercent = static_cast<int>(std::round(report.predictabilityScore * 100.0));
    std::cout << "    - Predictability:        " 
              << (predPercent < 25 ? Terminal::success(std::to_string(predPercent) + "% (Low)") :
                  predPercent < 50 ? Terminal::warning(std::to_string(predPercent) + "% (Moderate)") :
                                     Terminal::danger(std::to_string(predPercent) + "% (High)")) << "\n";

    std::ostringstream nashOss;
    nashOss << std::fixed << std::setprecision(2) << report.nashDistance << " TVD";
    std::cout << "    - Nash Distance:         " 
              << (report.nashDistance < 0.15 ? Terminal::success(nashOss.str()) : Terminal::warning(nashOss.str())) << "\n";

    int wsPercent = static_cast<int>(std::round(report.winStayRate * 100.0));
    int lsPercent = static_cast<int>(std::round(report.loseShiftRate * 100.0));
    std::cout << "    - Win-Stay Rate:         " << wsPercent << "%\n";
    std::cout << "    - Lose-Shift Rate:       " << lsPercent << "%\n";

    std::cout << "\n  " << Terminal::secondary("Playstyle Assessment:") << "\n";
    std::cout << "    " << Terminal::highlight(report.biasVerdict) << "\n";

    Terminal::printDivider();
}

} // namespace rps
