#include "GameEngine.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "RulesEngine.hpp"
#include "AsciiArt.hpp"
#include "GameTheoryAnalyzer.hpp"
#include "RandomGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace rps {

GameEngine::GameEngine(StatsManager& statsManager)
    : m_statsManager(statsManager) {}

RuleSet GameEngine::promptRuleSetSelection() {
    Terminal::printHeader("RULESET SELECTION", "Choose standard 3-way or extended 5-way rules");
    std::cout << "  " << Terminal::primary("[1]") << " Classic (Rock, Paper, Scissors)\n";
    std::cout << "  " << Terminal::secondary("[2]") << " RPSLS (Rock, Paper, Scissors, Lizard, Spock)\n\n";

    int choice = InputValidator::getIntInRange("  Select Ruleset [1-2]: ", 1, 2);
    return (choice == 1) ? RuleSet::Classic : RuleSet::RPSLS;
}

AiPersonality GameEngine::promptAiPersonalitySelection() {
    Terminal::printHeader("AI OPPONENT SELECTION", "Choose difficulty & algorithm");
    std::cout << "  " << Terminal::primary("[1]") << " Chaos Randomizer (Uniform random)\n";
    std::cout << "  " << Terminal::primary("[2]") << " Rocky Brute (Power move bias)\n";
    std::cout << "  " << Terminal::primary("[3]") << " Mirror Mimic (Echoes & counter-mirrors)\n";
    std::cout << "  " << Terminal::primary("[4]") << " WSLS Tactician (Win-Stay / Lose-Shift)\n";
    std::cout << "  " << Terminal::primary("[5]") << " Markov Oracle (2nd-order Markov predictive AI)\n\n";

    int choice = InputValidator::getIntInRange("  Select AI [1-5]: ", 1, 5);
    switch (choice) {
        case 1: return AiPersonality::Random;
        case 2: return AiPersonality::Brute;
        case 3: return AiPersonality::Mimic;
        case 4: return AiPersonality::TacticianWSLS;
        case 5: return AiPersonality::MarkovOracle;
        default: return AiPersonality::Random;
    }
}

void GameEngine::notifyAchievements(const std::vector<Achievement>& newlyUnlocked) {
    for (const auto& ach : newlyUnlocked) {
        Terminal::playBeep(m_statsManager.getProfile().soundEnabled);
        std::cout << "\n  " << Terminal::success("╔═══════════════════════════════════════════════════════════════════╗\n");
        std::cout << "  " << Terminal::success("║  ACHIEVEMENT UNLOCKED: ") 
                  << Terminal::highlight(ach.title) << " " << Terminal::secondary(ach.code) << Terminal::success(" ║\n");
        std::cout << "  " << Terminal::success("║  ") << Terminal::muted(ach.description) << Terminal::success(" ║\n");
        std::cout << "  " << Terminal::success("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    }
}

RoundResult GameEngine::executeRound(uint32_t roundNumber,
                                     RuleSet ruleSet,
                                     const std::string& p1Name,
                                     const std::string& p2Name,
                                     AiPredictor* aiPredictor,
                                     bool isPassAndPlay) {
    RoundResult result;
    result.roundNumber = roundNumber;
    result.p1Name = p1Name;
    result.p2Name = p2Name;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "\n  " << Terminal::primary("--- ROUND " + std::to_string(roundNumber) + " ---") << "\n";

    Move p1Move = Move::None;
    Move p2Move = Move::None;

    if (isPassAndPlay) {
        p1Move = InputValidator::getHiddenMoveInput(p1Name, ruleSet);
        p2Move = InputValidator::getHiddenMoveInput(p2Name, ruleSet);
    } else {
        std::string prompt = (ruleSet == RuleSet::Classic) ?
            "  Choose Move: [1] Rock (R) | [2] Paper (P) | [3] Scissors (S) | [Q] Quit: " :
            "  Choose Move: [1] Rock | [2] Paper | [3] Scissors | [4] Lizard | [5] Spock | [Q] Quit: ";

        p1Move = InputValidator::getMoveInput(prompt, ruleSet, true);
        if (p1Move == Move::None) {
            result.p1Move = Move::None;
            return result;
        }

        if (aiPredictor) {
            p2Move = aiPredictor->selectMove(ruleSet);
        } else {
            p2Move = RandomGenerator::getRandomMove(ruleSet);
        }
    }

    result.p1Move = p1Move;
    result.p2Move = p2Move;

    AsciiArt::playCountdown(ruleSet, !m_statsManager.getProfile().slowAnimations);

    result.outcome = RulesEngine::evaluate(p1Move, p2Move, ruleSet);
    result.verb = RulesEngine::getVerb((result.outcome == Outcome::Player1Win) ? p1Move : p2Move,
                                       (result.outcome == Outcome::Player1Win) ? p2Move : p1Move);
    result.commentary = RulesEngine::getRoundDescription(p1Move, p2Move, result.outcome, p1Name, p2Name);

    auto endTime = std::chrono::high_resolution_clock::now();
    result.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();

    AsciiArt::renderClash(p1Move, p2Move, result.outcome, p1Name, p2Name);
    Terminal::playBeep(m_statsManager.getProfile().soundEnabled);

    if (result.outcome == Outcome::Player1Win) {
        std::cout << "  " << Terminal::success("> " + result.commentary) << "\n";
    } else if (result.outcome == Outcome::Player2Win) {
        std::cout << "  " << Terminal::danger("> " + result.commentary) << "\n";
    } else {
        std::cout << "  " << Terminal::warning("> " + result.commentary) << "\n";
    }

    if (aiPredictor) {
        aiPredictor->recordRound(p1Move, p2Move, result.outcome);
    }

    return result;
}

void GameEngine::playQuickMatch() {
    Terminal::clearScreen();
    RuleSet ruleSet = promptRuleSetSelection();
    AiPersonality personality = promptAiPersonalitySelection();

    auto ai = AiPredictor::create(personality);
    std::string playerName = m_statsManager.getProfile().playerName;
    std::string aiName = ai->getName();

    Terminal::printHeader("QUICK MATCH DUEL", playerName + " VS " + aiName);
    std::cout << "  " << Terminal::muted("Profile: " + ai->getDescription()) << "\n";

    RoundResult res = executeRound(1, ruleSet, playerName, aiName, ai.get(), false);
    if (res.p1Move == Move::None) {
        std::cout << "  " << Terminal::muted("Match cancelled.") << "\n";
        return;
    }

    m_statsManager.recordRoundOutcome(res.p1Move, res.p2Move, res.outcome, ruleSet, personality);

    MatchSummary summary;
    summary.mode = GameMode::QuickMatch;
    summary.ruleSet = ruleSet;
    summary.p1Name = playerName;
    summary.p2Name = aiName;
    summary.p1Wins = (res.outcome == Outcome::Player1Win) ? 1 : 0;
    summary.p2Wins = (res.outcome == Outcome::Player2Win) ? 1 : 0;
    summary.ties = (res.outcome == Outcome::Tie) ? 1 : 0;
    summary.totalRounds = 1;
    summary.p1OverallWinner = (res.outcome == Outcome::Player1Win);
    summary.history.push_back(res);

    m_statsManager.recordMatchOutcome(summary);
    auto newlyUnlocked = m_statsManager.checkAchievements(&summary);
    notifyAchievements(newlyUnlocked);

    InputValidator::pause();
}

void GameEngine::playBestOfN() {
    Terminal::clearScreen();
    RuleSet ruleSet = promptRuleSetSelection();
    AiPersonality personality = promptAiPersonalitySelection();

    Terminal::printHeader("TOURNAMENT SERIES", "Select Match Length");
    std::cout << "  " << Terminal::primary("[1]") << " Best of 3 (First to 2 wins)\n";
    std::cout << "  " << Terminal::primary("[2]") << " Best of 5 (First to 3 wins)\n";
    std::cout << "  " << Terminal::primary("[3]") << " Best of 7 (First to 4 wins)\n";
    std::cout << "  " << Terminal::primary("[4]") << " Best of 11 (First to 6 wins)\n\n";

    int choice = InputValidator::getIntInRange("  Select Format [1-4]: ", 1, 4);
    int requiredWins = 2;
    int maxSeriesRounds = 3;
    if (choice == 1) { requiredWins = 2; maxSeriesRounds = 3; }
    else if (choice == 2) { requiredWins = 3; maxSeriesRounds = 5; }
    else if (choice == 3) { requiredWins = 4; maxSeriesRounds = 7; }
    else if (choice == 4) { requiredWins = 6; maxSeriesRounds = 11; }

    auto ai = AiPredictor::create(personality);
    std::string playerName = m_statsManager.getProfile().playerName;
    std::string aiName = ai->getName();

    MatchSummary summary;
    summary.mode = GameMode::BestOfN;
    summary.ruleSet = ruleSet;
    summary.p1Name = playerName;
    summary.p2Name = aiName;

    uint32_t currentRound = 1;

    while (summary.p1Wins < static_cast<uint32_t>(requiredWins) && 
           summary.p2Wins < static_cast<uint32_t>(requiredWins)) {
        
        Terminal::clearScreen();
        Terminal::printHeader("TOURNAMENT MATCH (BEST OF " + std::to_string(maxSeriesRounds) + ")",
                              playerName + " [" + std::to_string(summary.p1Wins) + "]  VS  [" + 
                              std::to_string(summary.p2Wins) + "] " + aiName);

        Terminal::printProgressBar(summary.p1Wins, requiredWins, 20, playerName + ":");
        Terminal::printProgressBar(summary.p2Wins, requiredWins, 20, aiName + ":");

        RoundResult res = executeRound(currentRound, ruleSet, playerName, aiName, ai.get(), false);
        if (res.p1Move == Move::None) {
            std::cout << "  " << Terminal::muted("Tournament forfeited.") << "\n";
            return;
        }

        if (res.outcome == Outcome::Player1Win) summary.p1Wins++;
        else if (res.outcome == Outcome::Player2Win) summary.p2Wins++;
        else summary.ties++;

        summary.history.push_back(res);
        summary.totalRounds++;
        currentRound++;

        m_statsManager.recordRoundOutcome(res.p1Move, res.p2Move, res.outcome, ruleSet, personality);
        auto roundAchievements = m_statsManager.checkAchievements();
        notifyAchievements(roundAchievements);

        Terminal::sleepMs(600);
    }

    summary.p1OverallWinner = (summary.p1Wins > summary.p2Wins);
    m_statsManager.recordMatchOutcome(summary);

    Terminal::clearScreen();
    if (summary.p1OverallWinner) {
        AsciiArt::printVictoryBanner();
        std::cout << "  " << Terminal::success("Series Won " + 
                                              std::to_string(summary.p1Wins) + " - " + std::to_string(summary.p2Wins) + "!") << "\n";
    } else {
        AsciiArt::printDefeatBanner();
        std::cout << "  " << Terminal::danger(aiName + " won the series " + 
                                              std::to_string(summary.p2Wins) + " - " + std::to_string(summary.p1Wins) + ".") << "\n";
    }

    auto newlyUnlocked = m_statsManager.checkAchievements(&summary);
    notifyAchievements(newlyUnlocked);

    if (InputValidator::getConfirmation("\n  View Game Theory analytics for this series?", true)) {
        auto report = GameTheoryAnalyzer::analyzeHistory(summary.history, ruleSet, true);
        GameTheoryAnalyzer::displayAnalyticsDashboard(report, ruleSet, playerName);
    }

    InputValidator::pause();
}

void GameEngine::playGauntlet() {
    Terminal::clearScreen();
    Terminal::printHeader("GAUNTLET MODE", "Beat 5 consecutive AI opponents in Best-of-3 series!");
    
    std::vector<AiPersonality> gauntletOrder = {
        AiPersonality::Random,
        AiPersonality::Brute,
        AiPersonality::Mimic,
        AiPersonality::TacticianWSLS,
        AiPersonality::MarkovOracle
    };

    RuleSet ruleSet = promptRuleSetSelection();
    std::string playerName = m_statsManager.getProfile().playerName;
    
    MatchSummary gauntletSummary;
    gauntletSummary.mode = GameMode::Gauntlet;
    gauntletSummary.ruleSet = ruleSet;
    gauntletSummary.p1Name = playerName;
    gauntletSummary.p2Name = "Gauntlet Opponents";

    bool completedAll = true;

    for (size_t stage = 0; stage < gauntletOrder.size(); ++stage) {
        auto ai = AiPredictor::create(gauntletOrder[stage]);
        std::string opponentName = ai->getName();

        Terminal::clearScreen();
        Terminal::printHeader("GAUNTLET STAGE " + std::to_string(stage + 1) + " / 5: " + opponentName, 
                              ai->getDescription());

        uint32_t p1Wins = 0;
        uint32_t aiWins = 0;
        uint32_t roundNum = 1;

        while (p1Wins < 2 && aiWins < 2) {
            RoundResult res = executeRound(roundNum, ruleSet, playerName, opponentName, ai.get(), false);
            if (res.p1Move == Move::None) {
                std::cout << "  " << Terminal::muted("Gauntlet abandoned.") << "\n";
                return;
            }

            if (res.outcome == Outcome::Player1Win) p1Wins++;
            else if (res.outcome == Outcome::Player2Win) aiWins++;

            gauntletSummary.history.push_back(res);
            m_statsManager.recordRoundOutcome(res.p1Move, res.p2Move, res.outcome, ruleSet, gauntletOrder[stage]);
            roundNum++;
            Terminal::sleepMs(500);
        }

        if (p1Wins > aiWins) {
            std::cout << "\n  " << Terminal::success("Stage " + std::to_string(stage + 1) + " Cleared! " + opponentName + " defeated.") << "\n";
            InputValidator::pause("Press Enter for next stage...");
        } else {
            std::cout << "\n  " << Terminal::danger("Gauntlet Failed against " + opponentName + ".") << "\n";
            completedAll = false;
            break;
        }
    }

    gauntletSummary.p1OverallWinner = completedAll;
    m_statsManager.recordMatchOutcome(gauntletSummary);

    Terminal::clearScreen();
    if (completedAll) {
        AsciiArt::printVictoryBanner();
        std::cout << "  " << Terminal::highlight("You conquered the entire Gauntlet ladder!") << "\n";
    } else {
        AsciiArt::printDefeatBanner();
    }

    auto newlyUnlocked = m_statsManager.checkAchievements(&gauntletSummary);
    notifyAchievements(newlyUnlocked);

    InputValidator::pause();
}

void GameEngine::playSurvival() {
    Terminal::clearScreen();
    Terminal::printHeader("ENDLESS SURVIVAL", "Start with 3 Lives. Streaks award bonus shields!");

    RuleSet ruleSet = promptRuleSetSelection();
    std::string playerName = m_statsManager.getProfile().playerName;

    int lives = 3;
    uint32_t wins = 0;
    uint32_t roundNum = 1;

    std::vector<RoundResult> history;

    while (lives > 0) {
        AiPersonality aiType = (wins < 3) ? AiPersonality::Random :
                               (wins < 7) ? AiPersonality::Brute :
                               (wins < 12) ? AiPersonality::Mimic :
                               (wins < 18) ? AiPersonality::TacticianWSLS :
                                             AiPersonality::MarkovOracle;

        auto ai = AiPredictor::create(aiType);
        std::string aiName = ai->getName();

        Terminal::clearScreen();
        std::ostringstream status;
        status << "Lives: ";
        for (int l = 0; l < lives; ++l) status << "<3 ";
        status << " | Streak: " << wins << " | Opponent: " << aiName;

        Terminal::printHeader("SURVIVAL ARENA", status.str());

        RoundResult res = executeRound(roundNum, ruleSet, playerName, aiName, ai.get(), false);
        if (res.p1Move == Move::None) {
            std::cout << "  " << Terminal::muted("Survival run ended.") << "\n";
            break;
        }

        history.push_back(res);
        m_statsManager.recordRoundOutcome(res.p1Move, res.p2Move, res.outcome, ruleSet, aiType);

        if (res.outcome == Outcome::Player1Win) {
            wins++;
            if (wins % 5 == 0) {
                lives++;
                std::cout << "\n  " << Terminal::highlight("Bonus life gained! Lives: " + std::to_string(lives)) << "\n";
            }
        } else if (res.outcome == Outcome::Player2Win) {
            lives--;
            std::cout << "\n  " << Terminal::danger("Life lost! Remaining: " + std::to_string(lives)) << "\n";
        }

        auto newlyUnlocked = m_statsManager.checkAchievements();
        notifyAchievements(newlyUnlocked);

        roundNum++;
        Terminal::sleepMs(500);
    }

    MatchSummary summary;
    summary.mode = GameMode::Survival;
    summary.ruleSet = ruleSet;
    summary.p1Name = playerName;
    summary.p2Name = "Survival AI Fleet";
    summary.p1Wins = wins;
    summary.history = history;

    m_statsManager.recordMatchOutcome(summary);

    Terminal::clearScreen();
    AsciiArt::printDefeatBanner();
    std::cout << "  " << Terminal::primary("Survival Run Ended. Total Wins: ") << Terminal::bold(std::to_string(wins)) << "\n";

    auto newlyUnlocked = m_statsManager.checkAchievements(&summary);
    notifyAchievements(newlyUnlocked);

    InputValidator::pause();
}

void GameEngine::playPassAndPlay() {
    Terminal::clearScreen();
    Terminal::printHeader("LOCAL 2-PLAYER PASS & PLAY", "Secret move input with simultaneous reveal");

    RuleSet ruleSet = promptRuleSetSelection();
    std::string p1Name = InputValidator::getString("  Enter Player 1 Name: ", 16, "Player 1");
    std::string p2Name = InputValidator::getString("  Enter Player 2 Name: ", 16, "Player 2");

    int rounds = InputValidator::getIntInRange("  Number of rounds to play (1-15): ", 1, 15);

    uint32_t p1Score = 0;
    uint32_t p2Score = 0;
    uint32_t ties = 0;

    std::vector<RoundResult> history;

    for (int r = 1; r <= rounds; ++r) {
        Terminal::clearScreen();
        Terminal::printHeader("PASS & PLAY: ROUND " + std::to_string(r) + " / " + std::to_string(rounds),
                              p1Name + " [" + std::to_string(p1Score) + "]  VS  [" + std::to_string(p2Score) + "] " + p2Name);

        RoundResult res = executeRound(r, ruleSet, p1Name, p2Name, nullptr, true);
        history.push_back(res);

        if (res.outcome == Outcome::Player1Win) p1Score++;
        else if (res.outcome == Outcome::Player2Win) p2Score++;
        else ties++;

        InputValidator::pause("Press Enter for next round...");
    }

    Terminal::clearScreen();
    Terminal::printHeader("PASS & PLAY RESULTS", p1Name + " vs " + p2Name);
    std::cout << "  " << Terminal::bold(p1Name) << " Score: " << Terminal::highlight(std::to_string(p1Score)) << "\n";
    std::cout << "  " << Terminal::bold(p2Name) << " Score: " << Terminal::highlight(std::to_string(p2Score)) << "\n";
    std::cout << "  Ties: " << Terminal::muted(std::to_string(ties)) << "\n\n";

    if (p1Score > p2Score) {
        AsciiArt::printVictoryBanner();
        std::cout << "  " << Terminal::success(p1Name + " won the showdown!") << "\n";
    } else if (p2Score > p1Score) {
        AsciiArt::printVictoryBanner();
        std::cout << "  " << Terminal::success(p2Name + " won the showdown!") << "\n";
    } else {
        AsciiArt::printDrawBanner();
        std::cout << "  " << Terminal::warning("Honorable Draw between " + p1Name + " and " + p2Name + "!") << "\n";
    }

    if (InputValidator::getConfirmation("\n  Inspect playstyle metrics for Player 1?", false)) {
        auto rep1 = GameTheoryAnalyzer::analyzeHistory(history, ruleSet, true);
        GameTheoryAnalyzer::displayAnalyticsDashboard(rep1, ruleSet, p1Name);
    }

    if (InputValidator::getConfirmation("\n  Inspect playstyle metrics for Player 2?", false)) {
        auto rep2 = GameTheoryAnalyzer::analyzeHistory(history, ruleSet, false);
        GameTheoryAnalyzer::displayAnalyticsDashboard(rep2, ruleSet, p2Name);
    }

    InputValidator::pause();
}

void GameEngine::playAiSimulation() {
    Terminal::clearScreen();
    Terminal::printHeader("AI VS AI SIMULATION", "Simulate algorithm matchups");

    RuleSet ruleSet = promptRuleSetSelection();

    std::cout << "\n  " << Terminal::secondary("Select AI 1 Algorithm:") << "\n";
    AiPersonality aiType1 = promptAiPersonalitySelection();

    std::cout << "\n  " << Terminal::secondary("Select AI 2 Algorithm:") << "\n";
    AiPersonality aiType2 = promptAiPersonalitySelection();

    int totalRounds = InputValidator::getIntInRange("  Rounds to simulate (10 - 5000): ", 10, 5000);
    bool liveView = (totalRounds <= 30) && InputValidator::getConfirmation("  Watch live animated round playback?", false);

    auto ai1 = AiPredictor::create(aiType1);
    auto ai2 = AiPredictor::create(aiType2);

    std::string name1 = "AI 1 (" + ai1->getName() + ")";
    std::string name2 = "AI 2 (" + ai2->getName() + ")";

    uint32_t wins1 = 0;
    uint32_t wins2 = 0;
    uint32_t draws = 0;

    std::vector<RoundResult> history;
    history.reserve(totalRounds);

    Terminal::clearScreen();
    Terminal::printHeader("SIMULATION RUNNING", name1 + " VS " + name2);

    for (int r = 1; r <= totalRounds; ++r) {
        Move m1 = ai1->selectMove(ruleSet);
        Move m2 = ai2->selectMove(ruleSet);

        Outcome outcome = RulesEngine::evaluate(m1, m2, ruleSet);
        
        RoundResult res;
        res.roundNumber = r;
        res.p1Move = m1;
        res.p2Move = m2;
        res.outcome = outcome;
        res.p1Name = name1;
        res.p2Name = name2;
        history.push_back(res);

        if (outcome == Outcome::Player1Win) wins1++;
        else if (outcome == Outcome::Player2Win) wins2++;
        else draws++;

        ai1->recordRound(m2, m1, outcome);
        
        Outcome flippedOutcome = (outcome == Outcome::Player1Win) ? Outcome::Player2Win :
                                 (outcome == Outcome::Player2Win) ? Outcome::Player1Win :
                                                                    Outcome::Tie;
        ai2->recordRound(m1, m2, flippedOutcome);

        if (liveView) {
            std::cout << "  Round " << std::setw(3) << r << ": " 
                      << InputValidator::moveToStringWithIcon(m1) << " vs " 
                      << InputValidator::moveToStringWithIcon(m2) << " -> " 
                      << ((outcome == Outcome::Player1Win) ? name1 + " wins" : 
                          (outcome == Outcome::Player2Win) ? name2 + " wins" : "Draw") << "\n";
            Terminal::sleepMs(60);
        } else if (r % (totalRounds / 10 + 1) == 0 || r == totalRounds) {
            Terminal::printProgressBar(r, totalRounds, 30, "Simulating:");
        }
    }

    Terminal::clearScreen();
    Terminal::printHeader("SIMULATION RESULTS", name1 + " VS " + name2);

    double winRate1 = static_cast<double>(wins1) / totalRounds * 100.0;
    double winRate2 = static_cast<double>(wins2) / totalRounds * 100.0;
    double drawRate = static_cast<double>(draws) / totalRounds * 100.0;

    std::cout << "  " << Terminal::primary(name1) << " Wins: " << Terminal::highlight(std::to_string(wins1)) 
              << " (" << std::fixed << std::setprecision(1) << winRate1 << "%)\n";
    std::cout << "  " << Terminal::secondary(name2) << " Wins: " << Terminal::highlight(std::to_string(wins2)) 
              << " (" << std::fixed << std::setprecision(1) << winRate2 << "%)\n";
    std::cout << "  " << Terminal::muted("Draws:") << " " << draws << " (" 
              << std::fixed << std::setprecision(1) << drawRate << "%)\n\n";

    auto report1 = GameTheoryAnalyzer::analyzeHistory(history, ruleSet, true);
    auto report2 = GameTheoryAnalyzer::analyzeHistory(history, ruleSet, false);

    GameTheoryAnalyzer::displayAnalyticsDashboard(report1, ruleSet, name1);
    GameTheoryAnalyzer::displayAnalyticsDashboard(report2, ruleSet, name2);

    InputValidator::pause();
}

} // namespace rps
