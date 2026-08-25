#include "StatsManager.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "GameTheoryAnalyzer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace rps {

StatsManager::StatsManager(const std::string& saveFilePath)
    : m_saveFilePath(saveFilePath) {
    initializeAchievements();
    loadProfile();
}

void StatsManager::initializeAchievements() {
    m_profile.achievements = {
        { AchievementId::FirstBlood,        { AchievementId::FirstBlood,        "[1B]", "First Blood",        "Win your first round in the arena", false, "" } },
        { AchievementId::ClassicMaster,     { AchievementId::ClassicMaster,     "[CM]", "Classic Master",     "Win 10 matches in Classic 3-Way mode", false, "" } },
        { AchievementId::SpockLogic,        { AchievementId::SpockLogic,        "[SP]", "Spock's Logic",      "Win 5 rounds using Spock", false, "" } },
        { AchievementId::LizardKing,        { AchievementId::LizardKing,        "[LK]", "Lizard King",        "Win 5 rounds using Lizard", false, "" } },
        { AchievementId::FlawlessSweep,     { AchievementId::FlawlessSweep,     "[FS]", "Flawless Sweep",     "Win a Best-of-5 tournament without dropping a round", false, "" } },
        { AchievementId::GauntletConqueror, { AchievementId::GauntletConqueror, "[GC]", "Gauntlet Conqueror", "Beat all 5 AI opponents in the Gauntlet ladder", false, "" } },
        { AchievementId::SurvivalWarrior,   { AchievementId::SurvivalWarrior,   "[SW]", "Survival Warrior",   "Reach a 10-win streak in Endless Survival", false, "" } },
        { AchievementId::EntropyMaster,     { AchievementId::EntropyMaster,     "[EM]", "Entropy Master",     "Maintain unpredictable entropy (> 90%) over 20+ rounds", false, "" } },
        { AchievementId::MindReader,        { AchievementId::MindReader,        "[MR]", "Mind Reader",        "Defeat the Markov Oracle in a Best-of-7 series", false, "" } },
        { AchievementId::CenturyFighter,    { AchievementId::CenturyFighter,    "[1C]", "Century Brawler",    "Play 100 total rounds across all game modes", false, "" } },
        { AchievementId::Tactician,         { AchievementId::Tactician,         "[TT]", "Master Tactician",   "Beat the WSLS Tactician without losing a round", false, "" } },
        { AchievementId::Grandmaster,       { AchievementId::Grandmaster,       "[GM]", "Grandmaster",        "Reach 10,000 career points", false, "" } }
    };
}

bool StatsManager::loadProfile() {
    std::ifstream file(m_saveFilePath);
    if (!file.is_open()) return false;

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
            continue;
        }

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);

        try {
            if (section == "PROFILE") {
                if (key == "playerName") m_profile.playerName = val;
                else if (key == "careerScore") m_profile.careerScore = static_cast<uint32_t>(std::stoul(val));
                else if (key == "totalRoundsPlayed") m_profile.totalRoundsPlayed = static_cast<uint32_t>(std::stoul(val));
                else if (key == "totalWins") m_profile.totalWins = static_cast<uint32_t>(std::stoul(val));
                else if (key == "totalLosses") m_profile.totalLosses = static_cast<uint32_t>(std::stoul(val));
                else if (key == "totalTies") m_profile.totalTies = static_cast<uint32_t>(std::stoul(val));
                else if (key == "currentStreak") m_profile.currentStreak = static_cast<uint32_t>(std::stoul(val));
                else if (key == "longestWinStreak") m_profile.longestWinStreak = static_cast<uint32_t>(std::stoul(val));
                else if (key == "quickMatchWins") m_profile.quickMatchWins = static_cast<uint32_t>(std::stoul(val));
                else if (key == "tournamentWins") m_profile.tournamentWins = static_cast<uint32_t>(std::stoul(val));
                else if (key == "gauntletClears") m_profile.gauntletClears = static_cast<uint32_t>(std::stoul(val));
                else if (key == "highestSurvivalRounds") m_profile.highestSurvivalRounds = static_cast<uint32_t>(std::stoul(val));
                else if (key == "rockCount") m_profile.rockCount = static_cast<uint32_t>(std::stoul(val));
                else if (key == "paperCount") m_profile.paperCount = static_cast<uint32_t>(std::stoul(val));
                else if (key == "scissorsCount") m_profile.scissorsCount = static_cast<uint32_t>(std::stoul(val));
                else if (key == "lizardCount") m_profile.lizardCount = static_cast<uint32_t>(std::stoul(val));
                else if (key == "spockCount") m_profile.spockCount = static_cast<uint32_t>(std::stoul(val));
                else if (key == "theme") m_profile.activeTheme = static_cast<Theme>(std::stoi(val));
                else if (key == "sound") m_profile.soundEnabled = (val == "1");
                else if (key == "slowAnimations") m_profile.slowAnimations = (val == "1");
            } else if (section == "ACHIEVEMENTS") {
                auto id = static_cast<AchievementId>(std::stoi(key));
                if (m_profile.achievements.find(id) != m_profile.achievements.end()) {
                    auto comma = val.find(',');
                    if (comma != std::string::npos) {
                        m_profile.achievements[id].unlocked = (val.substr(0, comma) == "1");
                        m_profile.achievements[id].unlockedDate = val.substr(comma + 1);
                    } else {
                        m_profile.achievements[id].unlocked = (val == "1");
                    }
                }
            }
        } catch (...) {
            // Ignore malformed lines and keep going
        }
    }

    Terminal::setTheme(m_profile.activeTheme);
    return true;
}

bool StatsManager::saveProfile() {
    std::ofstream file(m_saveFilePath, std::ios::trunc);
    if (!file.is_open()) return false;

    file << "# RPS Profile Save\n";
    file << "[PROFILE]\n";
    file << "playerName=" << m_profile.playerName << "\n";
    file << "careerScore=" << m_profile.careerScore << "\n";
    file << "totalRoundsPlayed=" << m_profile.totalRoundsPlayed << "\n";
    file << "totalWins=" << m_profile.totalWins << "\n";
    file << "totalLosses=" << m_profile.totalLosses << "\n";
    file << "totalTies=" << m_profile.totalTies << "\n";
    file << "currentStreak=" << m_profile.currentStreak << "\n";
    file << "longestWinStreak=" << m_profile.longestWinStreak << "\n";
    file << "quickMatchWins=" << m_profile.quickMatchWins << "\n";
    file << "tournamentWins=" << m_profile.tournamentWins << "\n";
    file << "gauntletClears=" << m_profile.gauntletClears << "\n";
    file << "highestSurvivalRounds=" << m_profile.highestSurvivalRounds << "\n";
    file << "rockCount=" << m_profile.rockCount << "\n";
    file << "paperCount=" << m_profile.paperCount << "\n";
    file << "scissorsCount=" << m_profile.scissorsCount << "\n";
    file << "lizardCount=" << m_profile.lizardCount << "\n";
    file << "spockCount=" << m_profile.spockCount << "\n";
    file << "theme=" << static_cast<int>(m_profile.activeTheme) << "\n";
    file << "sound=" << (m_profile.soundEnabled ? "1" : "0") << "\n";
    file << "slowAnimations=" << (m_profile.slowAnimations ? "1" : "0") << "\n";

    file << "\n[ACHIEVEMENTS]\n";
    for (const auto& [id, ach] : m_profile.achievements) {
        file << static_cast<int>(id) << "=" << (ach.unlocked ? "1" : "0") << "," << ach.unlockedDate << "\n";
    }

    return true;
}

void StatsManager::resetProfile() {
    Theme savedTheme = m_profile.activeTheme;
    bool sound = m_profile.soundEnabled;
    m_profile = PlayerProfile();
    m_profile.activeTheme = savedTheme;
    m_profile.soundEnabled = sound;
    initializeAchievements();
    saveProfile();
}

uint32_t StatsManager::calculateRoundScore(Outcome outcome, RuleSet ruleSet, AiPersonality aiLevel, uint32_t currentStreak) {
    if (outcome != Outcome::Player1Win) return 0;

    uint32_t base = (ruleSet == RuleSet::Classic) ? 50 : 80;
    
    double aiMult = 1.0;
    if (aiLevel == AiPersonality::Brute) aiMult = 1.1;
    else if (aiLevel == AiPersonality::Mimic) aiMult = 1.25;
    else if (aiLevel == AiPersonality::TacticianWSLS) aiMult = 1.5;
    else if (aiLevel == AiPersonality::MarkovOracle) aiMult = 2.0;

    double streakMult = 1.0;
    if (currentStreak >= 10) streakMult = 3.0;
    else if (currentStreak >= 5) streakMult = 2.0;
    else if (currentStreak >= 3) streakMult = 1.5;

    return static_cast<uint32_t>(base * aiMult * streakMult);
}

void StatsManager::recordRoundOutcome(Move playerMove, Move /*opponentMove*/, Outcome outcome, RuleSet ruleSet, AiPersonality aiLevel) {
    m_profile.totalRoundsPlayed++;

    if (playerMove == Move::Rock) m_profile.rockCount++;
    else if (playerMove == Move::Paper) m_profile.paperCount++;
    else if (playerMove == Move::Scissors) m_profile.scissorsCount++;
    else if (playerMove == Move::Lizard) m_profile.lizardCount++;
    else if (playerMove == Move::Spock) m_profile.spockCount++;

    if (outcome == Outcome::Player1Win) {
        m_profile.totalWins++;
        m_profile.currentStreak++;
        if (m_profile.currentStreak > m_profile.longestWinStreak) {
            m_profile.longestWinStreak = m_profile.currentStreak;
        }

        uint32_t pts = calculateRoundScore(outcome, ruleSet, aiLevel, m_profile.currentStreak);
        m_profile.careerScore += pts;
    } else if (outcome == Outcome::Player2Win) {
        m_profile.totalLosses++;
        m_profile.currentStreak = 0;
    } else {
        m_profile.totalTies++;
    }

    saveProfile();
}

void StatsManager::recordMatchOutcome(const MatchSummary& match) {
    if (match.p1OverallWinner) {
        if (match.mode == GameMode::QuickMatch) m_profile.quickMatchWins++;
        else if (match.mode == GameMode::BestOfN) m_profile.tournamentWins++;
        else if (match.mode == GameMode::Gauntlet) m_profile.gauntletClears++;
    }

    if (match.mode == GameMode::Survival) {
        if (match.p1Wins > m_profile.highestSurvivalRounds) {
            m_profile.highestSurvivalRounds = match.p1Wins;
        }
    }

    saveProfile();
}

bool StatsManager::unlockAchievement(AchievementId id) {
    auto it = m_profile.achievements.find(id);
    if (it != m_profile.achievements.end() && !it->second.unlocked) {
        it->second.unlocked = true;
        
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char buf[32];
        std::tm tmBuf{};
#ifdef _WIN32
        localtime_s(&tmBuf, &t);
#else
        localtime_r(&t, &tmBuf);
#endif
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tmBuf);
        it->second.unlockedDate = buf;

        saveProfile();
        return true;
    }
    return false;
}

bool StatsManager::isAchievementUnlocked(AchievementId id) const {
    auto it = m_profile.achievements.find(id);
    return (it != m_profile.achievements.end() && it->second.unlocked);
}

std::vector<Achievement> StatsManager::checkAchievements(const MatchSummary* completedMatch) {
    std::vector<Achievement> unlocked;

    auto tryUnlock = [&](AchievementId id) {
        if (unlockAchievement(id)) {
            unlocked.push_back(m_profile.achievements[id]);
        }
    };

    if (m_profile.totalWins >= 1) {
        tryUnlock(AchievementId::FirstBlood);
    }
    if ((m_profile.quickMatchWins + m_profile.tournamentWins) >= 10) {
        tryUnlock(AchievementId::ClassicMaster);
    }
    if (m_profile.spockCount >= 5) {
        tryUnlock(AchievementId::SpockLogic);
    }
    if (m_profile.lizardCount >= 5) {
        tryUnlock(AchievementId::LizardKing);
    }
    if (m_profile.totalRoundsPlayed >= 100) {
        tryUnlock(AchievementId::CenturyFighter);
    }
    if (m_profile.careerScore >= 10000) {
        tryUnlock(AchievementId::Grandmaster);
    }
    if (m_profile.longestWinStreak >= 10) {
        tryUnlock(AchievementId::SurvivalWarrior);
    }

    if (completedMatch) {
        if (completedMatch->mode == GameMode::BestOfN && completedMatch->p1OverallWinner) {
            if (completedMatch->p2Wins == 0 && completedMatch->p1Wins >= 3) {
                tryUnlock(AchievementId::FlawlessSweep);
            }
        }
        if (completedMatch->mode == GameMode::Gauntlet && completedMatch->p1OverallWinner) {
            tryUnlock(AchievementId::GauntletConqueror);
        }
    }

    return unlocked;
}

std::vector<MasteryRank> StatsManager::getAllRanks() {
    return {
        { "Novice Brawler",         "*",       0 },
        { "Tactical Duelist",       "**",     500 },
        { "Hand Master",            "***",    2000 },
        { "Psychology Reader",      "****",   5000 },
        { "Markov Dominator",       "*****",  10000 },
        { "Grandmaster of Hands",   "******", 15000 }
    };
}

MasteryRank StatsManager::getCurrentRank() const {
    auto ranks = getAllRanks();
    MasteryRank current = ranks.front();
    for (const auto& r : ranks) {
        if (m_profile.careerScore >= r.minScore) {
            current = r;
        }
    }
    return current;
}

void StatsManager::displayProfileCard() const {
    Terminal::printHeader("PLAYER PROFILE & ARENA STATS", m_profile.playerName);

    MasteryRank rank = getCurrentRank();
    double winRate = (m_profile.totalRoundsPlayed > 0) ? 
        (static_cast<double>(m_profile.totalWins) / m_profile.totalRoundsPlayed * 100.0) : 0.0;

    std::cout << "  " << Terminal::primary("Player: ") << Terminal::bold(m_profile.playerName)
              << " | " << Terminal::secondary("Rank: ") << Terminal::highlight(rank.title + " " + rank.stars) 
              << " | " << Terminal::accent("Score: ") << m_profile.careerScore << " pts\n\n";

    std::vector<std::string> lines;
    std::ostringstream s1, s2, s3, s4;

    s1 << "Total Rounds:  " << std::left << std::setw(8) << m_profile.totalRoundsPlayed
       << " Wins: " << std::setw(8) << m_profile.totalWins 
       << " Win Rate: " << std::fixed << std::setprecision(1) << winRate << "%";
    lines.push_back(s1.str());

    s2 << "Losses:        " << std::left << std::setw(8) << m_profile.totalLosses
       << " Ties: " << std::setw(8) << m_profile.totalTies
       << " Current Streak: " << m_profile.currentStreak;
    lines.push_back(s2.str());

    s3 << "Best Streak:   " << std::left << std::setw(8) << m_profile.longestWinStreak
       << " Tourneys: " << std::setw(4) << m_profile.tournamentWins
       << " Gauntlet Clears: " << m_profile.gauntletClears;
    lines.push_back(s3.str());

    s4 << "Survival High: " << m_profile.highestSurvivalRounds << " rounds";
    lines.push_back(s4.str());

    Terminal::printBox(lines, 72, false);

    std::cout << "\n  " << Terminal::secondary("Lifetime Move Breakdown:") << "\n";
    uint32_t totalThrows = m_profile.rockCount + m_profile.paperCount + m_profile.scissorsCount + 
                           m_profile.lizardCount + m_profile.spockCount;

    auto printBar = [&](const std::string& name, uint32_t count) {
        double pct = (totalThrows > 0) ? (static_cast<double>(count) / totalThrows * 100.0) : 0.0;
        std::cout << "    - " << std::left << std::setw(14) << name << ": " 
                  << std::right << std::setw(5) << count << " (" << std::fixed << std::setprecision(1) << pct << "%)\n";
    };

    printBar("Rock", m_profile.rockCount);
    printBar("Paper", m_profile.paperCount);
    printBar("Scissors", m_profile.scissorsCount);
    printBar("Lizard", m_profile.lizardCount);
    printBar("Spock", m_profile.spockCount);

    Terminal::printDivider();
}

void StatsManager::displayAchievementsGrid() const {
    Terminal::printHeader("ACHIEVEMENTS VAULT", "Unlocked Feats & Badges");

    int count = 0;
    for (const auto& [id, ach] : m_profile.achievements) {
        if (ach.unlocked) count++;
    }

    Terminal::printProgressBar(count, static_cast<int>(m_profile.achievements.size()), 35, "Unlocked:");
    std::cout << "\n";

    for (const auto& [id, ach] : m_profile.achievements) {
        std::string badge = ach.unlocked ? Terminal::success(ach.code + " *") : Terminal::muted(ach.code + " .");
        std::string title = ach.unlocked ? Terminal::bold(ach.title) : Terminal::muted(ach.title);
        std::string status = ach.unlocked ? Terminal::secondary("[" + ach.unlockedDate + "]") : Terminal::muted("[LOCKED]");

        std::cout << "  " << badge << " " << std::left << std::setw(22) << title << " " 
                  << std::left << std::setw(42) << ach.description << " " << status << "\n";
    }

    Terminal::printDivider();
}

void StatsManager::displayLeaderboardRanks() const {
    Terminal::printHeader("MASTERY RANKS", "Career progression thresholds");

    auto ranks = getAllRanks();
    MasteryRank current = getCurrentRank();

    for (const auto& r : ranks) {
        bool isCurrent = (r.title == current.title);
        std::string prefix = isCurrent ? Terminal::success(" > [CURRENT] ") : "   [       ] ";
        std::string title = isCurrent ? Terminal::highlight(r.title + " " + r.stars) : Terminal::primary(r.title + " " + r.stars);

        std::cout << prefix << std::left << std::setw(30) << title 
                  << " Min Score: " << std::right << std::setw(6) << r.minScore << " pts\n";
    }

    Terminal::printDivider();
}

} // namespace rps
