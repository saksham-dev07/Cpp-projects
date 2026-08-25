#include "Terminal.hpp"
#include "InputValidator.hpp"
#include "StatsManager.hpp"
#include "GameEngine.hpp"
#include "AsciiArt.hpp"
#include "RandomGenerator.hpp"
#include <iostream>

using namespace rps;

void displayThemeMenu(StatsManager& statsManager) {
    Terminal::clearScreen();
    Terminal::printHeader("THEME SELECTOR", "Choose an ANSI color palette");

    std::cout << "  " << Terminal::primary("[1] Neon Cyberpunk") << " (Cyan / Magenta)\n";
    std::cout << "  " << Terminal::primary("[2] Matrix Emerald") << " (Green / Lime)\n";
    std::cout << "  " << Terminal::primary("[3] Sunset Crimson") << " (Amber / Crimson)\n";
    std::cout << "  " << Terminal::primary("[4] Deep Ocean") << " (Azure / Turquoise)\n";
    std::cout << "  " << Terminal::primary("[5] Royal Amethyst") << " (Purple / Rose)\n";
    std::cout << "  " << Terminal::primary("[6] Monochrome Clean") << " (White / Silver)\n\n";

    int choice = InputValidator::getIntInRange("  Select Theme [1-6]: ", 1, 6);
    Theme selected = static_cast<Theme>(choice - 1);
    statsManager.getProfile().activeTheme = selected;
    Terminal::setTheme(selected);
    statsManager.saveProfile();

    std::cout << "\n  " << Terminal::success("Theme updated to: " + Terminal::getThemeName(selected)) << "\n";
    InputValidator::pause();
}

void displaySettingsMenu(StatsManager& statsManager) {
    while (true) {
        Terminal::clearScreen();
        Terminal::printHeader("SETTINGS", "Preferences and configuration");

        auto& prof = statsManager.getProfile();
        std::cout << "  " << Terminal::primary("[1]") << " Player Name: " << Terminal::highlight(prof.playerName) << "\n";
        std::cout << "  " << Terminal::primary("[2]") << " Color Theme: " << Terminal::secondary(Terminal::getThemeName(prof.activeTheme)) << "\n";
        std::cout << "  " << Terminal::primary("[3]") << " Sound Effects: " << (prof.soundEnabled ? Terminal::success("ON") : Terminal::danger("OFF")) << "\n";
        std::cout << "  " << Terminal::primary("[4]") << " Animation Pace: " << (prof.slowAnimations ? Terminal::success("Cinematic") : Terminal::warning("Fast")) << "\n";
        std::cout << "  " << Terminal::primary("[5]") << " " << Terminal::danger("Reset Stats & Achievements") << "\n";
        std::cout << "  " << Terminal::muted("[0] Back to Main Menu\n\n");

        int choice = InputValidator::getIntInRange("  Select Option [0-5]: ", 0, 5);
        if (choice == 0) break;

        switch (choice) {
            case 1: {
                std::string newName = InputValidator::getString("  Enter new player name: ", 16, prof.playerName);
                prof.playerName = newName;
                statsManager.saveProfile();
                std::cout << "  " << Terminal::success("Player name saved: " + newName) << "\n";
                InputValidator::pause();
                break;
            }
            case 2:
                displayThemeMenu(statsManager);
                break;
            case 3:
                prof.soundEnabled = !prof.soundEnabled;
                statsManager.saveProfile();
                Terminal::playBeep(prof.soundEnabled);
                std::cout << "  " << Terminal::info("Sound: " + std::string(prof.soundEnabled ? "Enabled" : "Disabled")) << "\n";
                InputValidator::pause();
                break;
            case 4:
                prof.slowAnimations = !prof.slowAnimations;
                statsManager.saveProfile();
                std::cout << "  " << Terminal::info("Animations: " + std::string(prof.slowAnimations ? "Cinematic" : "Fast")) << "\n";
                InputValidator::pause();
                break;
            case 5:
                if (InputValidator::getConfirmation("  Are you sure you want to reset all profile data?", false)) {
                    statsManager.resetProfile();
                    std::cout << "  " << Terminal::danger("Profile data reset.") << "\n";
                    InputValidator::pause();
                }
                break;
        }
    }
}

void displayRulesMenu() {
    Terminal::clearScreen();
    AsciiArt::printRulesDiagram(RuleSet::Classic);
    std::cout << "\n";
    AsciiArt::printRulesDiagram(RuleSet::RPSLS);
    InputValidator::pause();
}

int main() {
    Terminal::init();
    RandomGenerator::init();

    StatsManager statsManager("rps_stats.dat");
    GameEngine gameEngine(statsManager);

    while (true) {
        Terminal::clearScreen();
        AsciiArt::printMainBanner();

        auto rank = statsManager.getCurrentRank();
        const auto& prof = statsManager.getProfile();

        std::cout << "  " << Terminal::primary("Player: ") << Terminal::bold(prof.playerName)
                  << " | " << Terminal::secondary("Rank: ") << Terminal::highlight(rank.title + " " + rank.stars)
                  << " | " << Terminal::accent("Score: ") << prof.careerScore << " pts\n";
        Terminal::printDivider();

        std::cout << "  " << Terminal::primary("  --- GAME MODES ---") << "\n";
        std::cout << "  " << Terminal::primary("[1]") << " Quick Match (" << Terminal::muted("Single round vs AI") << ")\n";
        std::cout << "  " << Terminal::primary("[2]") << " Tournament (" << Terminal::muted("Best of 3 / 5 / 7 / 11") << ")\n";
        std::cout << "  " << Terminal::primary("[3]") << " Gauntlet Ladder (" << Terminal::muted("5 AI boss fights") << ")\n";
        std::cout << "  " << Terminal::primary("[4]") << " Endless Survival (" << Terminal::muted("3 Lives, Streak multiplier") << ")\n";
        std::cout << "  " << Terminal::primary("[5]") << " Pass & Play (" << Terminal::muted("Local 2-Player, secret input") << ")\n";
        std::cout << "  " << Terminal::primary("[6]") << " AI vs AI Simulation (" << Terminal::muted("Automated match analysis") << ")\n\n";

        std::cout << "  " << Terminal::secondary("  --- PROFILE & STATS ---") << "\n";
        std::cout << "  " << Terminal::secondary("[7]") << " Rules & Payoff Matrices\n";
        std::cout << "  " << Terminal::secondary("[8]") << " Player Career Stats\n";
        std::cout << "  " << Terminal::secondary("[9]") << " Achievements Vault\n";
        std::cout << "  " << Terminal::secondary("[10]") << " Mastery Ladder\n";
        std::cout << "  " << Terminal::secondary("[11]") << " Settings & Themes\n\n";

        std::cout << "  " << Terminal::muted("[0] Exit Arena\n\n");

        int choice = InputValidator::getIntInRange("  Select Option [0-11]: ", 0, 11);
        if (choice == 0) {
            Terminal::clearScreen();
            std::cout << "\n  " << Terminal::highlight("Thanks for playing!") << "\n\n";
            break;
        }

        switch (choice) {
            case 1: gameEngine.playQuickMatch(); break;
            case 2: gameEngine.playBestOfN(); break;
            case 3: gameEngine.playGauntlet(); break;
            case 4: gameEngine.playSurvival(); break;
            case 5: gameEngine.playPassAndPlay(); break;
            case 6: gameEngine.playAiSimulation(); break;
            case 7: displayRulesMenu(); break;
            case 8:
                Terminal::clearScreen();
                statsManager.displayProfileCard();
                InputValidator::pause();
                break;
            case 9:
                Terminal::clearScreen();
                statsManager.displayAchievementsGrid();
                InputValidator::pause();
                break;
            case 10:
                Terminal::clearScreen();
                statsManager.displayLeaderboardRanks();
                InputValidator::pause();
                break;
            case 11:
                displaySettingsMenu(statsManager);
                break;
        }
    }

    return 0;
}
