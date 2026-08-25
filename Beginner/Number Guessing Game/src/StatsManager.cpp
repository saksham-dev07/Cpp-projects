#include "StatsManager.hpp"
#include "RandomGenerator.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cmath>

namespace GuessGame {

    StatsManager::StatsManager(const std::string& savePath)
        : m_savePath(savePath) {
        initializeDefaultAchievements();
        loadProfile();
    }

    void StatsManager::initializeDefaultAchievements() {
        m_profile.achievements = {
            { AchievementId::FirstWin, "First Steps", "Win your first number guessing game", "[*]", false, "" },
            { AchievementId::LuckyOne, "Lucky 777", "Guess the secret number on the very first attempt", "[7]", false, "" },
            { AchievementId::BinaryProdigy, "Binary Prodigy", "Win within theoretical log2(N) optimal attempts", "[B]", false, "" },
            { AchievementId::SpeedDemon, "Speed Demon", "Win a game in under 10 seconds", "[S]", false, "" },
            { AchievementId::StreakTitan, "Streak Titan", "Achieve a 5-win streak without losing", "[W]", false, "" },
            { AchievementId::SurvivalMaster, "Survival Veteran", "Survive Extreme difficulty with limited attempts", "[V]", false, "" },
            { AchievementId::AntiCheatSleuth, "Anti-Cheat Sleuth", "Catch the system / trigger contradiction detector in Reverse AI mode", "[!]", false, "" },
            { AchievementId::MastermindAce, "Mastermind Ace", "Crack Bulls & Cows 4-digit code in 6 attempts or fewer", "[M]", false, "" },
            { AchievementId::MathScholar, "Math Scholar", "Use math clues and achieve victory", "[+]", false, "" },
            { AchievementId::CenturyClub, "Century Club", "Play a total of 100 rounds", "[C]", false, "" },
            { AchievementId::FlawlessDuel, "Duel Grand Champion", "Win a Duel game against AI or Player", "[D]", false, "" },
            { AchievementId::Grandmaster, "Oracle Grandmaster", "Reach an overall score of 10,000+ points", "[G]", false, "" }
        };
    }

    std::string StatsManager::getCurrentDateTime() const {
        std::time_t now = std::time(nullptr);
        std::tm localTm;
#ifdef _WIN32
        localtime_s(&localTm, &now);
#else
        localtime_r(&now, &localTm);
#endif
        std::stringstream ss;
        ss << std::put_time(&localTm, "%Y-%m-%d %H:%M");
        return ss.str();
    }

    bool StatsManager::loadProfile() {
        std::ifstream file(m_savePath);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        std::string section;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) { return !std::isspace(ch); }));
            line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), line.end());

            if (line.empty() || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }

            std::stringstream ss(line);
            if (section == "PROFILE") {
                std::string key;
                if (std::getline(ss, key, '=')) {
                    std::string val;
                    std::getline(ss, val);
                    if (key == "username") m_profile.username = val;
                    else if (key == "totalGamesPlayed") m_profile.totalGamesPlayed = std::stoi(val);
                    else if (key == "totalWins") m_profile.totalWins = std::stoi(val);
                    else if (key == "currentStreak") m_profile.currentStreak = std::stoi(val);
                    else if (key == "maxStreak") m_profile.maxStreak = std::stoi(val);
                    else if (key == "totalScore") m_profile.totalScore = std::stoi(val);
                    else if (key == "totalTimeSeconds") m_profile.totalTimeSeconds = std::stod(val);
                    else if (key == "hintsUsedCount") m_profile.hintsUsedCount = std::stoi(val);
                    else if (key == "activeTheme") m_profile.activeTheme = static_cast<ThemeColor>(std::stoi(val));
                    else if (key == "soundEnabled") m_profile.soundEnabled = (val == "1" || val == "true");
                }
            } else if (section == "ACHIEVEMENTS") {
                int idVal;
                int unlockedVal;
                std::string date;
                if (ss >> idVal >> unlockedVal) {
                    std::getline(ss, date);
                    // Trim date
                    date.erase(date.begin(), std::find_if(date.begin(), date.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                    for (auto& ach : m_profile.achievements) {
                        if (static_cast<int>(ach.id) == idVal) {
                            ach.unlocked = (unlockedVal == 1);
                            ach.unlockedAt = date;
                            break;
                        }
                    }
                }
            } else if (section == "HIGHSCORES") {
                HighScoreEntry entry;
                int modeInt, diffInt;
                if (ss >> entry.playerName >> modeInt >> diffInt >> entry.score >> entry.attempts >> entry.timeSeconds) {
                    entry.mode = static_cast<GameMode>(modeInt);
                    entry.difficulty = static_cast<Difficulty>(diffInt);
                    std::getline(ss, entry.date);
                    entry.date.erase(entry.date.begin(), std::find_if(entry.date.begin(), entry.date.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                    m_profile.highScores.push_back(entry);
                }
            }
        }

        return true;
    }

    bool StatsManager::saveProfile() const {
        std::ofstream file(m_savePath);
        if (!file.is_open()) return false;

        file << "# Number Guessing Game Save Data\n\n";

        file << "[PROFILE]\n";
        file << "username=" << m_profile.username << "\n";
        file << "totalGamesPlayed=" << m_profile.totalGamesPlayed << "\n";
        file << "totalWins=" << m_profile.totalWins << "\n";
        file << "currentStreak=" << m_profile.currentStreak << "\n";
        file << "maxStreak=" << m_profile.maxStreak << "\n";
        file << "totalScore=" << m_profile.totalScore << "\n";
        file << "totalTimeSeconds=" << m_profile.totalTimeSeconds << "\n";
        file << "hintsUsedCount=" << m_profile.hintsUsedCount << "\n";
        file << "activeTheme=" << static_cast<int>(m_profile.activeTheme) << "\n";
        file << "soundEnabled=" << (m_profile.soundEnabled ? "1" : "0") << "\n\n";

        file << "[ACHIEVEMENTS]\n";
        for (const auto& ach : m_profile.achievements) {
            file << static_cast<int>(ach.id) << " " << (ach.unlocked ? 1 : 0) << " " << (ach.unlocked ? ach.unlockedAt : "locked") << "\n";
        }
        file << "\n";

        file << "[HIGHSCORES]\n";
        for (const auto& hs : m_profile.highScores) {
            file << hs.playerName << " "
                 << static_cast<int>(hs.mode) << " "
                 << static_cast<int>(hs.difficulty) << " "
                 << hs.score << " "
                 << hs.attempts << " "
                 << hs.timeSeconds << " "
                 << hs.date << "\n";
        }

        return true;
    }

    void StatsManager::resetProfile() {
        std::string user = m_profile.username;
        ThemeColor theme = m_profile.activeTheme;
        bool sound = m_profile.soundEnabled;

        m_profile = PlayerProfile();
        m_profile.username = user;
        m_profile.activeTheme = theme;
        m_profile.soundEnabled = sound;
        initializeDefaultAchievements();
        saveProfile();
    }

    int StatsManager::calculateScore(const GameRoundSummary& summary) const {
        if (!summary.won) return 0;

        int basePoints = 200;
        switch (summary.difficulty) {
            case Difficulty::Easy: basePoints = 100; break;
            case Difficulty::Medium: basePoints = 250; break;
            case Difficulty::Hard: basePoints = 600; break;
            case Difficulty::Extreme: basePoints = 1200; break;
            case Difficulty::Custom: basePoints = 200; break;
        }

        if (summary.mode == GameMode::BullsAndCows) {
            basePoints = 800;
        }

        // Efficiency bonus: compare attempts used vs expected optimal attempts
        int optimal = std::max(1, summary.maxAttemptsAllowed > 0 ? summary.maxAttemptsAllowed : 7);
        double efficiencyRatio = static_cast<double>(optimal) / std::max(1, summary.attemptsUsed);
        
        int attemptBonus = static_cast<int>(basePoints * std::min(2.5, efficiencyRatio));

        // Speed bonus (more points if answered quickly)
        int speedBonus = 0;
        if (summary.elapsedSeconds > 0 && summary.elapsedSeconds < 60.0) {
            speedBonus = static_cast<int>((60.0 - summary.elapsedSeconds) * 5.0);
        }

        // Hint penalty
        int hintPenalty = summary.hintsUsed * 40;

        // Streak multiplier
        double streakMult = 1.0 + std::min(0.50, m_profile.currentStreak * 0.10);

        int totalRoundScore = static_cast<int>((attemptBonus + speedBonus - hintPenalty) * streakMult);
        return std::max(50, totalRoundScore);
    }

    std::vector<AchievementId> StatsManager::processRoundResults(GameRoundSummary& summary) {
        m_profile.totalGamesPlayed++;
        m_profile.totalTimeSeconds += summary.elapsedSeconds;
        m_profile.hintsUsedCount += summary.hintsUsed;

        if (summary.won) {
            m_profile.totalWins++;
            m_profile.currentStreak++;
            m_profile.maxStreak = std::max(m_profile.maxStreak, m_profile.currentStreak);
            summary.scoreEarned = calculateScore(summary);
            m_profile.totalScore += summary.scoreEarned;

            // Add High Score
            HighScoreEntry hs;
            hs.playerName = m_profile.username;
            hs.mode = summary.mode;
            hs.difficulty = summary.difficulty;
            hs.score = summary.scoreEarned;
            hs.attempts = summary.attemptsUsed;
            hs.timeSeconds = summary.elapsedSeconds;
            hs.date = getCurrentDateTime();
            addHighScore(hs);
        } else {
            m_profile.currentStreak = 0;
            summary.scoreEarned = 0;
        }

        std::vector<AchievementId> newUnlocks;
        checkAndUnlockAchievements(summary, newUnlocks);
        summary.newlyUnlockedAchievements = newUnlocks;

        saveProfile();
        return newUnlocks;
    }

    void StatsManager::checkAndUnlockAchievements(GameRoundSummary& summary, std::vector<AchievementId>& newUnlocks) {
        auto tryUnlock = [&](AchievementId id) {
            for (auto& ach : m_profile.achievements) {
                if (ach.id == id && !ach.unlocked) {
                    ach.unlocked = true;
                    ach.unlockedAt = getCurrentDateTime();
                    newUnlocks.push_back(id);
                }
            }
        };

        if (summary.won) {
            tryUnlock(AchievementId::FirstWin);

            if (summary.attemptsUsed == 1) {
                tryUnlock(AchievementId::LuckyOne);
            }

            int optimal = summary.maxAttemptsAllowed > 0 ? summary.maxAttemptsAllowed : 7;
            if (summary.attemptsUsed <= optimal) {
                tryUnlock(AchievementId::BinaryProdigy);
            }

            if (summary.elapsedSeconds > 0.0 && summary.elapsedSeconds <= 10.0) {
                tryUnlock(AchievementId::SpeedDemon);
            }

            if (m_profile.currentStreak >= 5) {
                tryUnlock(AchievementId::StreakTitan);
            }

            if (summary.mode == GameMode::Survival && summary.difficulty == Difficulty::Extreme) {
                tryUnlock(AchievementId::SurvivalMaster);
            }

            if (summary.mode == GameMode::BullsAndCows && summary.attemptsUsed <= 6) {
                tryUnlock(AchievementId::MastermindAce);
            }

            if (summary.hintsUsed >= 2) {
                tryUnlock(AchievementId::MathScholar);
            }

            if (summary.mode == GameMode::Duel) {
                tryUnlock(AchievementId::FlawlessDuel);
            }
        }

        if (summary.contradictionCaught) {
            tryUnlock(AchievementId::AntiCheatSleuth);
        }

        if (m_profile.totalGamesPlayed >= 100) {
            tryUnlock(AchievementId::CenturyClub);
        }

        if (m_profile.totalScore >= 10000) {
            tryUnlock(AchievementId::Grandmaster);
        }
    }

    void StatsManager::addHighScore(const HighScoreEntry& entry) {
        m_profile.highScores.push_back(entry);
        std::sort(m_profile.highScores.begin(), m_profile.highScores.end(), [](const HighScoreEntry& a, const HighScoreEntry& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.timeSeconds < b.timeSeconds;
        });
        if (m_profile.highScores.size() > 50) {
            m_profile.highScores.resize(50);
        }
    }

    std::vector<HighScoreEntry> StatsManager::getTopHighScores(size_t limit) const {
        std::vector<HighScoreEntry> result = m_profile.highScores;
        if (result.size() > limit) {
            result.resize(limit);
        }
        return result;
    }

    int StatsManager::getUnlockedAchievementCount() const {
        int count = 0;
        for (const auto& ach : m_profile.achievements) {
            if (ach.unlocked) count++;
        }
        return count;
    }

} // namespace GuessGame
