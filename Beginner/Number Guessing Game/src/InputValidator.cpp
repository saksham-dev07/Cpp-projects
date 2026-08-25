#include "InputValidator.hpp"
#include "Terminal.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <limits>

namespace GuessGame {

    namespace {
        std::string trim(std::string_view str) {
            auto start = str.find_first_not_of(" \t\r\n");
            if (start == std::string_view::npos) return "";
            auto end = str.find_last_not_of(" \t\r\n");
            return std::string(str.substr(start, end - start + 1));
        }
    }

    void InputValidator::flushInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int InputValidator::getInt(const std::string& prompt, int minVal, int maxVal) {
        const auto& c = Terminal::colors();
        while (true) {
            std::cout << c.accent << prompt << c.reset << " ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                std::cin.clear();
                return minVal;
            }

            line = trim(line);
            if (line.empty()) {
                std::cout << "  " << c.error << "[!] Please enter a number between " 
                          << minVal << " and " << maxVal << "." << c.reset << "\n";
                continue;
            }

            std::stringstream ss(line);
            long long val;
            char extra;
            if ((ss >> val) && !(ss >> extra)) {
                if (val >= minVal && val <= maxVal) {
                    return static_cast<int>(val);
                }
                std::cout << "  " << c.warning << "[!] Value must be in range [" 
                          << minVal << " - " << maxVal << "]." << c.reset << "\n";
            } else {
                std::cout << "  " << c.error << "[!] Invalid number. Please enter digits only." << c.reset << "\n";
            }
        }
    }

    int InputValidator::getIntWithSpecial(const std::string& prompt, int minVal, int maxVal, int specialVal, const std::string& specialHelp) {
        const auto& c = Terminal::colors();
        while (true) {
            std::cout << c.accent << prompt << c.reset << " ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                std::cin.clear();
                return 0;
            }

            line = trim(line);
            if (line.empty()) {
                std::cout << "  " << c.error << "[!] Input cannot be empty." << c.reset << "\n";
                continue;
            }

            std::stringstream ss(line);
            long long val;
            char extra;
            if ((ss >> val) && !(ss >> extra)) {
                if (val == specialVal || val == 0) {
                    return static_cast<int>(val);
                }
                if (val >= minVal && val <= maxVal) {
                    return static_cast<int>(val);
                }
                
                std::cout << "  " << c.warning << "[!] Value must be in range [" << minVal << " - " << maxVal << "]";
                if (!specialHelp.empty()) {
                    std::cout << " or (" << specialVal << " for " << specialHelp << ", 0 to quit)";
                }
                std::cout << "." << c.reset << "\n";
            } else {
                std::cout << "  " << c.error << "[!] Invalid input. Please enter a valid number." << c.reset << "\n";
            }
        }
    }

    std::string InputValidator::getBullsCowsGuess(const std::string& prompt) {
        const auto& c = Terminal::colors();
        while (true) {
            std::cout << c.accent << prompt << c.reset << " ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                std::cin.clear();
                return "1234";
            }

            line = trim(line);
            if (line.length() != 4) {
                std::cout << "  " << c.error << "[!] Secret code must be exactly 4 digits." << c.reset << "\n";
                continue;
            }

            bool allDigits = std::all_of(line.begin(), line.end(), [](unsigned char ch) {
                return std::isdigit(ch);
            });

            if (!allDigits) {
                std::cout << "  " << c.error << "[!] Code must contain numbers (0-9) only." << c.reset << "\n";
                continue;
            }

            std::unordered_set<char> seen;
            bool hasDuplicates = std::any_of(line.begin(), line.end(), [&](char ch) {
                return !seen.insert(ch).second;
            });

            if (hasDuplicates) {
                std::cout << "  " << c.warning << "[!] All 4 digits must be distinct (no duplicate numbers)!" << c.reset << "\n";
                continue;
            }

            return line;
        }
    }

    char InputValidator::getChar(const std::string& prompt, std::string_view validChars) {
        const auto& c = Terminal::colors();
        std::string upperValid(validChars);
        for (char& ch : upperValid) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));

        while (true) {
            std::cout << c.accent << prompt << c.reset << " ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                std::cin.clear();
                return upperValid.empty() ? 'A' : upperValid[0];
            }

            line = trim(line);
            if (line.length() == 1) {
                char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(line[0])));
                if (upperValid.find(ch) != std::string::npos) {
                    return ch;
                }
            }

            std::cout << "  " << c.error << "[!] Please choose one of: [";
            for (size_t i = 0; i < upperValid.size(); ++i) {
                std::cout << upperValid[i] << (i + 1 < upperValid.size() ? "/" : "");
            }
            std::cout << "]" << c.reset << "\n";
        }
    }

    std::string InputValidator::getString(const std::string& prompt, bool allowEmpty) {
        const auto& c = Terminal::colors();
        while (true) {
            std::cout << c.accent << prompt << c.reset << " ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                std::cin.clear();
                return allowEmpty ? "" : "Player";
            }

            line = trim(line);
            if (line.empty() && !allowEmpty) {
                std::cout << "  " << c.error << "[!] Input cannot be empty." << c.reset << "\n";
                continue;
            }

            return line;
        }
    }

    bool InputValidator::getYesNo(const std::string& prompt, bool defaultYes) {
        std::string suffix = defaultYes ? " [Y/n]:" : " [y/N]:";
        char choice = getChar(prompt + suffix, "YN");
        return choice == 'Y';
    }

} // namespace GuessGame
