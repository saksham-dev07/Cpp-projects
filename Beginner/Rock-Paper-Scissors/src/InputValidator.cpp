#include "InputValidator.hpp"
#include "Terminal.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace rps {

namespace {
    std::string trimString(const std::string& str) {
        auto first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        auto last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    std::string toUpperCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        return str;
    }
}

int InputValidator::getIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return minVal;
        }

        std::string cleaned = trimString(line);
        if (cleaned.empty()) {
            std::cout << "  " << Terminal::danger("Please enter a valid number.") << "\n";
            continue;
        }

        try {
            size_t pos = 0;
            int value = std::stoi(cleaned, &pos);
            if (pos != cleaned.length()) {
                std::cout << "  " << Terminal::danger("Please enter an integer without extra characters.") << "\n";
                continue;
            }
            if (value < minVal || value > maxVal) {
                std::cout << "  " << Terminal::danger("Please choose between " + std::to_string(minVal) + " and " + std::to_string(maxVal) + ".") << "\n";
                continue;
            }
            return value;
        } catch (...) {
            std::cout << "  " << Terminal::danger("Invalid input. Try again.") << "\n";
        }
    }
}

std::string InputValidator::getString(const std::string& prompt, size_t maxLength, const std::string& defaultVal) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return defaultVal;
        }

        std::string cleaned = trimString(line);
        if (cleaned.empty()) {
            return defaultVal;
        }
        if (cleaned.length() > maxLength) {
            std::cout << "  " << Terminal::danger("Maximum length is " + std::to_string(maxLength) + " characters.") << "\n";
            continue;
        }
        return cleaned;
    }
}

bool InputValidator::getConfirmation(const std::string& prompt, bool defaultYes) {
    std::string hint = defaultYes ? "[Y/n]" : "[y/N]";
    while (true) {
        std::cout << prompt << " " << Terminal::muted(hint) << ": ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return defaultYes;
        }

        std::string cleaned = toUpperCase(trimString(line));
        if (cleaned.empty()) return defaultYes;
        if (cleaned == "Y" || cleaned == "YES") return true;
        if (cleaned == "N" || cleaned == "NO") return false;

        std::cout << "  " << Terminal::danger("Please enter 'y' or 'n'.") << "\n";
    }
}

Move InputValidator::parseMoveString(const std::string& input, RuleSet ruleSet) {
    std::string s = toUpperCase(trimString(input));
    if (s.empty()) return Move::None;

    if (s == "1" || s == "R" || s == "ROCK" || s == "[1] ROCK") return Move::Rock;
    if (s == "2" || s == "P" || s == "PAPER" || s == "[2] PAPER") return Move::Paper;
    if (s == "3" || s == "S" || s == "SCISSORS" || s == "SCISSOR" || s == "[3] SCISSORS") return Move::Scissors;

    if (ruleSet == RuleSet::RPSLS) {
        if (s == "4" || s == "L" || s == "LIZARD" || s == "[4] LIZARD") return Move::Lizard;
        if (s == "5" || s == "K" || s == "SPOCK" || s == "[5] SPOCK") return Move::Spock;
    }

    return Move::None;
}

Move InputValidator::getMoveInput(const std::string& prompt, RuleSet ruleSet, bool allowQuit) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            return Move::None;
        }

        std::string cleaned = toUpperCase(trimString(line));
        if (allowQuit && (cleaned == "Q" || cleaned == "QUIT" || cleaned == "EXIT")) {
            return Move::None;
        }

        Move move = parseMoveString(cleaned, ruleSet);
        if (move != Move::None) {
            return move;
        }

        if (ruleSet == RuleSet::Classic) {
            std::cout << "  " << Terminal::danger("Choose [1] Rock (R), [2] Paper (P), or [3] Scissors (S)" + std::string(allowQuit ? ", or [Q] to quit." : ".")) << "\n";
        } else {
            std::cout << "  " << Terminal::danger("Choose [1] Rock, [2] Paper, [3] Scissors, [4] Lizard, or [5] Spock" + std::string(allowQuit ? ", or [Q] to quit." : ".")) << "\n";
        }
    }
}

Move InputValidator::getHiddenMoveInput(const std::string& playerName, RuleSet ruleSet) {
    while (true) {
        std::string prompt = "  " + Terminal::secondary(playerName) + ", enter secret move (e.g. 1/R, 2/P, 3/S" + 
                             (ruleSet == RuleSet::RPSLS ? ", 4/L, 5/K" : "") + "): ";
        std::string raw = Terminal::readHiddenInput(prompt);
        Move move = parseMoveString(raw, ruleSet);
        if (move != Move::None) {
            return move;
        }
        std::cout << "  " << Terminal::danger("Invalid move. Try again.") << "\n";
    }
}

void InputValidator::pause(const std::string& prompt) {
    std::cout << "\n  " << Terminal::muted(prompt) << std::flush;
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string InputValidator::moveToString(Move move) {
    switch (move) {
        case Move::Rock:     return "Rock";
        case Move::Paper:    return "Paper";
        case Move::Scissors: return "Scissors";
        case Move::Lizard:   return "Lizard";
        case Move::Spock:    return "Spock";
        default:             return "None";
    }
}

std::string InputValidator::moveToStringWithIcon(Move move) {
    switch (move) {
        case Move::Rock:     return "✊ Rock";
        case Move::Paper:    return "✋ Paper";
        case Move::Scissors: return "✌️ Scissors";
        case Move::Lizard:   return "🦎 Lizard";
        case Move::Spock:    return "🖖 Spock";
        default:             return "❓ None";
    }
}

} // namespace rps
