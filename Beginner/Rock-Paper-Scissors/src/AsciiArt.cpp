#include "AsciiArt.hpp"
#include "Terminal.hpp"
#include "InputValidator.hpp"
#include <iostream>
#include <iomanip>

namespace rps {

void AsciiArt::printMainBanner() {
    std::cout << "\n";
    std::cout << Terminal::primary("  ╔═════════════════════════════════════════════════════════════════════════╗\n");
    std::cout << Terminal::secondary("  ║   ____   ___   ____ _  __   ____   _    ____  _____ ____    ____  ____  ║\n");
    std::cout << Terminal::secondary("  ║  |  _ \\ / _ \\ / ___| |/ /  |  _ \\ / \\  |  _ \\| ____|  _ \\  / ___||  _ \\ ║\n");
    std::cout << Terminal::secondary("  ║  | |_) | | | | |   | ' /   | |_) / _ \\ | |_) |  _| | |_) | \\___ \\| |_) |║\n");
    std::cout << Terminal::secondary("  ║  |  _ <| |_| | |___| . \\   |  __/ ___ \\|  __/| |___|  _ <   ___) |  __/ ║\n");
    std::cout << Terminal::secondary("  ║  |_| \\_\\\\___/ \\____|_|\\_\\  |_| /_/   \\_\\_|   |_____|_| \\_\\ |____/|_|    ║\n");
    std::cout << Terminal::primary("  ╚═════════════════════════════════════════════════════════════════════════╝\n");
    std::cout << "               " << Terminal::highlight("Rock-Paper-Scissors: Tournament & AI Arena") << "\n\n";
}

std::vector<std::string> AsciiArt::getHandArt(Move move, bool facingRight) {
    if (facingRight) {
        switch (move) {
            case Move::Rock:
                return {
                    "    _______     ",
                    "---'   ____)    ",
                    "      (_____)   ",
                    "      (_____)   ",
                    "      (____)    ",
                    "---.__(___)     "
                };
            case Move::Paper:
                return {
                    "    _______     ",
                    "---'   ____)____",
                    "          ______)",
                    "          _______)",
                    "         _______)",
                    "---.__________) "
                };
            case Move::Scissors:
                return {
                    "    _______     ",
                    "---'   ____)____",
                    "          ______)",
                    "       __________)",
                    "      (____)    ",
                    "---.__(___)     "
                };
            case Move::Lizard:
                return {
                    "    _______     ",
                    "---'   _  ___)  ",
                    "      (_)(___)  ",
                    "      (______)  ",
                    "       (_____)  ",
                    "---.__(___)     "
                };
            case Move::Spock:
                return {
                    "    _______     ",
                    "---'   ____)____",
                    "      (___  ___)",
                    "      (___  ___)",
                    "       (_______)",
                    "---.__________) "
                };
            default:
                return {
                    "    _______     ",
                    "---'   ????)    ",
                    "      (?????)   ",
                    "      (?????)   ",
                    "      (????)    ",
                    "---.__(???)     "
                };
        }
    } else {
        switch (move) {
            case Move::Rock:
                return {
                    "     _______    ",
                    "    (____   '---",
                    "   (_____)      ",
                    "   (_____)      ",
                    "    (____)      ",
                    "     (___)__.---"
                };
            case Move::Paper:
                return {
                    "     _______    ",
                    "____(____   '---",
                    "(______         ",
                    "(_______        ",
                    " (_______       ",
                    "  (__________.---"
                };
            case Move::Scissors:
                return {
                    "     _______    ",
                    "____(____   '---",
                    "(______         ",
                    "(__________     ",
                    "    (____)      ",
                    "     (___)__.---"
                };
            case Move::Lizard:
                return {
                    "     _______    ",
                    "  (___  _   '---",
                    "  (___)(_)      ",
                    "  (______)      ",
                    "   (_____)      ",
                    "     (___)__.---"
                };
            case Move::Spock:
                return {
                    "     _______    ",
                    "____(____   '---",
                    "(___  ___)      ",
                    "(___  ___)      ",
                    " (_______)      ",
                    "  (__________.---"
                };
            default:
                return {
                    "     _______    ",
                    "    (????   '---",
                    "   (?????)      ",
                    "   (?????)      ",
                    "    (????)      ",
                    "     (???)__.---"
                };
        }
    }
}

void AsciiArt::playCountdown(RuleSet ruleSet, bool fast) {
    int delayMs = fast ? 120 : 300;

    std::cout << "\n  ";
    std::cout << Terminal::primary("ROCK...") << std::flush;
    Terminal::sleepMs(delayMs);

    std::cout << " " << Terminal::secondary("PAPER...") << std::flush;
    Terminal::sleepMs(delayMs);

    std::cout << " " << Terminal::accent("SCISSORS...") << std::flush;
    Terminal::sleepMs(delayMs);

    if (ruleSet == RuleSet::RPSLS) {
        std::cout << " " << Terminal::info("LIZARD...") << std::flush;
        Terminal::sleepMs(delayMs);

        std::cout << " " << Terminal::warning("SPOCK...") << std::flush;
        Terminal::sleepMs(delayMs);
    }

    std::cout << " " << Terminal::bold(Terminal::danger("SHOOT!")) << "\n\n" << std::flush;
    Terminal::sleepMs(fast ? 80 : 200);
}

void AsciiArt::renderClash(Move p1Move, Move p2Move, Outcome outcome,
                          const std::string& p1Name,
                          const std::string& p2Name) {
    auto p1Art = getHandArt(p1Move, true);
    auto p2Art = getHandArt(p2Move, false);

    std::string p1Color = (outcome == Outcome::Player1Win) ? Terminal::success(p1Name) :
                          (outcome == Outcome::Player2Win) ? Terminal::danger(p1Name) :
                                                             Terminal::warning(p1Name);

    std::string p2Color = (outcome == Outcome::Player2Win) ? Terminal::success(p2Name) :
                          (outcome == Outcome::Player1Win) ? Terminal::danger(p2Name) :
                                                             Terminal::warning(p2Name);

    std::cout << "     " << std::left << std::setw(20) << p1Color 
              << "          " << Terminal::highlight("VS") 
              << "          " << std::right << std::setw(20) << p2Color << "\n";

    std::cout << "     " << std::left << std::setw(22) << Terminal::muted("(" + InputValidator::moveToString(p1Move) + ")")
              << "                     " 
              << std::right << std::setw(22) << Terminal::muted("(" + InputValidator::moveToString(p2Move) + ")") << "\n";

    std::cout << Terminal::muted("  -----------------------------------------------------------------------\n");

    for (size_t i = 0; i < 6; ++i) {
        std::string line1 = (i < p1Art.size()) ? p1Art[i] : "                ";
        std::string line2 = (i < p2Art.size()) ? p2Art[i] : "                ";

        std::string center = "         ";
        if (i == 2) center = "   VS    ";
        if (i == 3) center = "  CLASH  ";

        std::string col1 = (outcome == Outcome::Player1Win) ? Terminal::success(line1) :
                           (outcome == Outcome::Player2Win) ? Terminal::muted(line1) :
                                                              Terminal::info(line1);

        std::string col2 = (outcome == Outcome::Player2Win) ? Terminal::success(line2) :
                           (outcome == Outcome::Player1Win) ? Terminal::muted(line2) :
                                                              Terminal::info(line2);

        std::cout << "  " << col1 << "     " << Terminal::accent(center) << "     " << col2 << "\n";
    }

    std::cout << Terminal::muted("  -----------------------------------------------------------------------\n\n");
}

void AsciiArt::printRulesDiagram(RuleSet ruleSet) {
    if (ruleSet == RuleSet::Classic) {
        Terminal::printHeader("CLASSIC 3-WAY RULES", "Rock, Paper, Scissors");
        std::vector<std::string> lines = {
            "  Rock     beats   Scissors  (\"crushes\")",
            "  Scissors beats   Paper     (\"cuts\")",
            "  Paper    beats   Rock      (\"covers\")",
            "",
            "  - Symmetric 0-sum game.",
            "  - Optimal Mixed Nash Equilibrium: 1/3 probability per move."
        };
        Terminal::printBox(lines, 72, false);
    } else {
        Terminal::printHeader("EXTENDED 5-WAY RPSLS RULES", "Sam Kass 5-Way Dynamics");
        std::vector<std::string> lines = {
            "  Scissors  cuts       Paper",
            "  Paper     covers     Rock",
            "  Rock      crushes    Lizard",
            "  Lizard    poisons    Spock",
            "  Spock     smashes    Scissors",
            "  Scissors  decapitatesLizard",
            "  Lizard    eats       Paper",
            "  Paper     disproves  Spock",
            "  Spock     vaporizes  Rock",
            "  Rock      crushes    Scissors",
            "",
            "  - 5-Cycle Directed Graph: each move beats 2 and loses to 2.",
            "  - Optimal Mixed Nash Equilibrium: Uniform 20% (1/5) probability."
        };
        Terminal::printBox(lines, 72, false);
    }
}

void AsciiArt::printVictoryBanner() {
    std::cout << Terminal::success("  ╔═══════════════════════════════════════════════════════════════════╗\n");
    std::cout << Terminal::success("  ║                      *** VICTORY! ***                             ║\n");
    std::cout << Terminal::success("  ╚═══════════════════════════════════════════════════════════════════╝\n\n");
}

void AsciiArt::printDefeatBanner() {
    std::cout << Terminal::danger("  ╔═══════════════════════════════════════════════════════════════════╗\n");
    std::cout << Terminal::danger("  ║                       *** DEFEAT ***                              ║\n");
    std::cout << Terminal::danger("  ╚═══════════════════════════════════════════════════════════════════╝\n\n");
}

void AsciiArt::printDrawBanner() {
    std::cout << Terminal::warning("  ╔═══════════════════════════════════════════════════════════════════╗\n");
    std::cout << Terminal::warning("  ║                      *** STALEMATE ***                            ║\n");
    std::cout << Terminal::warning("  ╚═══════════════════════════════════════════════════════════════════╝\n\n");
}

} // namespace rps
