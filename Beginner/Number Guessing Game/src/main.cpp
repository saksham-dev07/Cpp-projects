#include "Types.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "GameEngine.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace GuessGame;

void displayProfile(StatsManager& stats) {
    Terminal::clearScreen();
    Terminal::printHeader("PLAYER DOSSIER & PERFORMANCE STATS", "Lifetime gameplay metrics and ranking");

    const auto& c = Terminal::colors();
    const auto& p = stats.getProfile();

    std::stringstream winRateSs;
    winRateSs << std::fixed << std::setprecision(1) << p.getWinRate() << "%";

    std::vector<std::string> profileLines = {
        "Agent / Pilot Name    : " + p.username,
        "Current Mastery Rank  : " + p.getRankTitle(),
        "Total Games Played    : " + std::to_string(p.totalGamesPlayed),
        "Total Victories       : " + std::to_string(p.totalWins),
        "Lifetime Win Rate     : " + winRateSs.str(),
        "Current Win Streak    : " + std::to_string(p.currentStreak) + " wins",
        "All-Time Best Streak  : " + std::to_string(p.maxStreak) + " wins",
        "Total Career Score    : " + Terminal::formatScore(p.totalScore),
        "Tactical Hints Used   : " + std::to_string(p.hintsUsedCount),
        "Total Playtime        : " + Terminal::formatTime(p.totalTimeSeconds),
        "Achievements Unlocked : " + std::to_string(stats.getUnlockedAchievementCount()) + " / 12"
    };

    Terminal::printBox(profileLines, c.primary);
    Terminal::pausePrompt();
}

void displayAchievements(StatsManager& stats) {
    Terminal::clearScreen();
    Terminal::printHeader("ACHIEVEMENTS VAULT", "Unlock special badges through exceptional gameplay");

    const auto& c = Terminal::colors();
    auto achievements = stats.getAllAchievements();

    std::vector<std::string> headers = {"Status", "Icon", "Achievement Title", "Requirement", "Unlocked Date"};
    std::vector<std::vector<std::string>> rows;

    for (const auto& ach : achievements) {
        rows.push_back({
            ach.unlocked ? "[UNLOCKED]" : "[LOCKED]",
            ach.icon,
            ach.title,
            ach.description,
            ach.unlocked ? ach.unlockedAt : "---"
        });
    }

    Terminal::printTable(headers, rows);
    Terminal::pausePrompt();
}

void displayHighScores(StatsManager& stats) {
    Terminal::clearScreen();
    Terminal::printHeader("HALL OF FAME (TOP SCORES)", "Global leaderboards across all game modes");

    const auto& c = Terminal::colors();
    auto highScores = stats.getTopHighScores(15);

    if (highScores.empty()) {
        std::cout << "  " << c.muted << "No high scores registered yet. Play a game to record your legendary scores!" << c.reset << "\n";
    } else {
        std::vector<std::string> headers = {"Rank", "Player", "Game Mode", "Difficulty", "Score", "Guesses", "Time", "Date"};
        std::vector<std::vector<std::string>> rows;

        for (size_t i = 0; i < highScores.size(); ++i) {
            const auto& hs = highScores[i];
            std::string modeStr = (hs.mode == GameMode::Classic ? "Classic" :
                                   hs.mode == GameMode::Survival ? "Survival" :
                                   hs.mode == GameMode::TimeAttack ? "TimeAttack" :
                                   hs.mode == GameMode::ReverseAI ? "Reverse AI" :
                                   hs.mode == GameMode::Duel ? "Duel" : "Bulls&Cows");

            std::string diffStr = (hs.difficulty == Difficulty::Easy ? "Easy" :
                                   hs.difficulty == Difficulty::Medium ? "Medium" :
                                   hs.difficulty == Difficulty::Hard ? "Hard" :
                                   hs.difficulty == Difficulty::Extreme ? "Extreme" : "Custom");

            rows.push_back({
                "#" + std::to_string(i + 1),
                hs.playerName,
                modeStr,
                diffStr,
                std::to_string(hs.score) + " pts",
                std::to_string(hs.attempts),
                Terminal::formatTime(hs.timeSeconds),
                hs.date
            });
        }
        Terminal::printTable(headers, rows);
    }

    Terminal::pausePrompt();
}

void displaySettings(GameEngine& engine) {
    auto& stats = engine.getStatsManager();
    auto& profile = stats.getProfile();

    while (true) {
        Terminal::clearScreen();
        Terminal::printHeader("SYSTEM SETTINGS & THEMES", "Customize your interface aesthetic and preferences");

        const auto& c = Terminal::colors();
        std::cout << "  " << c.primary << "[1] " << c.accent << "Change Active Theme   " << c.muted << "-> Current: " 
                  << (profile.activeTheme == ThemeColor::NeonCyberpunk ? "Neon Cyberpunk" :
                      profile.activeTheme == ThemeColor::RetroEmerald ? "Retro Emerald" :
                      profile.activeTheme == ThemeColor::SunsetAmber ? "Sunset Amber" :
                      profile.activeTheme == ThemeColor::OceanBlue ? "Ocean Blue" : "Monochrome") << c.reset << "\n";

        std::cout << "  " << c.primary << "[2] " << c.accent << "Toggle Sound Effects  " << c.muted << "-> Current: " 
                  << (profile.soundEnabled ? "[ON] Enabled" : "[OFF] Muted") << c.reset << "\n";

        std::cout << "  " << c.primary << "[3] " << c.accent << "Rename Player Profile " << c.muted << "-> Current: " << profile.username << c.reset << "\n";
        std::cout << "  " << c.primary << "[4] " << c.error << "Reset All Stats       " << c.muted << "-> Reset profile and achievements" << c.reset << "\n";
        std::cout << "  " << c.primary << "[0] " << c.accent << "Return to Main Menu" << c.reset << "\n\n";

        int choice = InputValidator::getInt("Select an option (0-4):", 0, 4);

        if (choice == 0) break;

        if (choice == 1) {
            Terminal::clearScreen();
            Terminal::printHeader("SELECT COLOR THEME", "Experience the game in fresh visual aesthetics");
            std::cout << "  [1] Neon Cyberpunk (Cyan / Magenta / Yellow)\n";
            std::cout << "  [2] Retro Emerald (Matrix Green / Gold)\n";
            std::cout << "  [3] Sunset Amber (Warm Orange / Sunset Crimson)\n";
            std::cout << "  [4] Deep Ocean (Sky Blue / Azure / Turquoise)\n";
            std::cout << "  [5] Monochrome Minimalist (Crisp White / Silver)\n\n";
            int tc = InputValidator::getInt("Choose theme (1-5):", 1, 5);
            ThemeColor newTheme = (tc == 1) ? ThemeColor::NeonCyberpunk :
                                  (tc == 2) ? ThemeColor::RetroEmerald :
                                  (tc == 3) ? ThemeColor::SunsetAmber :
                                  (tc == 4) ? ThemeColor::OceanBlue : ThemeColor::Monochrome;
            profile.activeTheme = newTheme;
            Terminal::setTheme(newTheme);
            stats.saveProfile();
        } else if (choice == 2) {
            profile.soundEnabled = !profile.soundEnabled;
            stats.saveProfile();
            if (profile.soundEnabled) Terminal::beep(true);
        } else if (choice == 3) {
            profile.username = InputValidator::getString("Enter new player callsign/username:");
            stats.saveProfile();
        } else if (choice == 4) {
            if (InputValidator::getYesNo("Are you SURE you want to reset all stats, scores, and achievements?", false)) {
                stats.resetProfile();
                std::cout << "\n  " << c.success << "[✓] Profile and statistics have been reset." << c.reset << "\n";
                Terminal::pausePrompt();
            }
        }
    }
}

int main() {
    Terminal::init();
    GameEngine engine;

    const auto& c = Terminal::colors();
    const size_t INNER_WIDTH = 75;
    auto repeatUtf8 = [](const std::string& pattern, size_t count) {
        std::string result;
        result.reserve(pattern.size() * count);
        for (size_t i = 0; i < count; ++i) result += pattern;
        return result;
    };

    auto printSectionHeader = [&](const std::string& title) {
        std::string centered = Terminal::center(title, INNER_WIDTH);
        std::cout << "  " << Terminal::colors().primary << "║" << Terminal::colors().accent << centered << Terminal::colors().primary << "║" << Terminal::colors().reset << "\n";
    };

    auto printMenuLine = [&](const std::string& key, const std::string& name, const std::string& desc, const std::string& keyColor) {
        const auto& col = Terminal::colors();
        std::string lineContent = "  [" + key + "] " + Terminal::padRight(name, 21) + "-> " + desc;
        size_t padLen = (INNER_WIDTH > lineContent.length()) ? (INNER_WIDTH - lineContent.length()) : 0;
        std::cout << "  " << col.primary << "║" << keyColor << "  [" << key + "] " << col.accent << Terminal::padRight(name, 21) 
                  << col.muted << "-> " << desc 
                  << std::string(padLen, ' ')
                  << col.primary << "║" << col.reset << "\n";
    };

    while (true) {
        Terminal::clearScreen();
        Terminal::printBanner();

        const auto& p = engine.getStatsManager().getProfile();

        std::cout << "  " << c.muted << "Pilot: " << c.accent << p.username << c.muted 
                  << " | Rank: " << c.highlight << p.getRankTitle() 
                  << c.muted << " | Score: " << c.accent << p.totalScore << " pts" 
                  << c.muted << " | Streak: " << c.success << p.currentStreak << "W" << c.reset << "\n\n";

        std::cout << "  " << c.primary << "╔" << repeatUtf8("═", INNER_WIDTH) << "╗" << c.reset << "\n";
        printSectionHeader("ARENA GAME MODES");
        std::cout << "  " << c.primary << "╠" << repeatUtf8("═", INNER_WIDTH) << "╣" << c.reset << "\n";
        printMenuLine("1", "Classic Mode", "Standard range guessing with proximity clues", c.accent);
        printMenuLine("2", "Survival Mode", "Hardcore limited attempts log2(N) challenge", c.accent);
        printMenuLine("3", "Time Attack", "Speedrun against ticking countdown clock", c.accent);
        printMenuLine("4", "Reverse AI Guesser", "Think of a number; AI guesses + Anti-Cheat", c.accent);
        printMenuLine("5", "Duel Arena", "Turn-based race against AI or 2nd Player", c.accent);
        printMenuLine("6", "Bulls & Cows", "Mastermind 4-digit secret code challenge", c.accent);
        std::cout << "  " << c.primary << "╠" << repeatUtf8("═", INNER_WIDTH) << "╣" << c.reset << "\n";
        printSectionHeader("PROFILE & SYSTEM");
        std::cout << "  " << c.primary << "╠" << repeatUtf8("═", INNER_WIDTH) << "╣" << c.reset << "\n";
        printMenuLine("7", "Player Dossier", "View win rates, streaks, and rank status", c.secondary);
        printMenuLine("8", "Achievements Vault", "Inspect 12 unlockable mastery badges", c.secondary);
        printMenuLine("9", "Hall of Fame", "High scores leaderboard", c.secondary);
        printMenuLine("S", "Settings & Themes", "Customize themes, sound, and callsign", c.secondary);
        printMenuLine("0", "Exit Game", "Save progress and return to terminal", c.error);
        std::cout << "  " << c.primary << "╚" << repeatUtf8("═", INNER_WIDTH) << "╝" << c.reset << "\n\n";

        char choice = InputValidator::getChar("Enter selection (0-9, S):", "0123456789S");

        switch (choice) {
            case '1': engine.playClassic(); break;
            case '2': engine.playSurvival(); break;
            case '3': engine.playTimeAttack(); break;
            case '4': engine.playReverseAi(); break;
            case '5': engine.playDuel(); break;
            case '6': engine.playBullsAndCows(); break;
            case '7': displayProfile(engine.getStatsManager()); break;
            case '8': displayAchievements(engine.getStatsManager()); break;
            case '9': displayHighScores(engine.getStatsManager()); break;
            case 'S': displaySettings(engine); break;
            case '0':
                Terminal::clearScreen();
                std::cout << "\n  " << c.highlight << "Thanks for playing Number Guessing Arena! Until next time!" << c.reset << "\n\n";
                return 0;
        }
    }

    return 0;
}
