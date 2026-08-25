#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "UI.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace UI {

static std::string repeat(const std::string& pattern, int count) {
    if (count <= 0) return "";
    std::string res;
    res.reserve(pattern.size() * count);
    for (int i = 0; i < count; ++i) {
        res += pattern;
    }
    return res;
}

void enableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void pause(const std::string& prompt) {
    std::cout << "\n" << Color::DIM << prompt << Color::RESET;
    std::cin.get();
}

void printHeader(const std::string& title, const std::string& subtitle) {
    std::cout << "\n";
    std::cout << Color::BRIGHT_CYAN << "┌────────────────────────────────────────────────────────────┐" << Color::RESET << "\n";
    
    int padTitle = (58 - static_cast<int>(title.length())) / 2;
    if (padTitle < 0) padTitle = 0;
    std::cout << Color::BRIGHT_CYAN << "│" << Color::RESET 
              << std::string(padTitle, ' ') 
              << Color::BOLD << Color::BRIGHT_WHITE << title << Color::RESET 
              << std::string(std::max(0, 58 - padTitle - static_cast<int>(title.length())), ' ') 
              << Color::BRIGHT_CYAN << "│" << Color::RESET << "\n";

    if (!subtitle.empty()) {
        int padSub = (58 - static_cast<int>(subtitle.length())) / 2;
        if (padSub < 0) padSub = 0;
        std::cout << Color::BRIGHT_CYAN << "│" << Color::RESET 
                  << std::string(padSub, ' ') 
                  << Color::DIM << subtitle << Color::RESET 
                  << std::string(std::max(0, 58 - padSub - static_cast<int>(subtitle.length())), ' ') 
                  << Color::BRIGHT_CYAN << "│" << Color::RESET << "\n";
    }

    std::cout << Color::BRIGHT_CYAN << "└────────────────────────────────────────────────────────────┘" << Color::RESET << "\n\n";
}

void printSection(const std::string& title) {
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_YELLOW << "--- " << title << " ---" << Color::RESET << "\n";
}

void printBox(const std::string& title, const std::vector<std::string>& lines, int minWidth) {
    int maxLen = static_cast<int>(title.length());
    for (const auto& line : lines) {
        if (static_cast<int>(line.length()) > maxLen) {
            maxLen = static_cast<int>(line.length());
        }
    }
    int width = std::max(minWidth, maxLen + 4);

    std::cout << Color::CYAN << "┌─ " << Color::BOLD << Color::BRIGHT_WHITE << title << Color::RESET 
              << Color::CYAN << " " << repeat("─", std::max(0, width - static_cast<int>(title.length()) - 5)) << "┐" << Color::RESET << "\n";

    for (const auto& line : lines) {
        std::cout << Color::CYAN << "│ " << Color::RESET << line 
                  << std::string(std::max(0, width - static_cast<int>(line.length()) - 3), ' ') 
                  << Color::CYAN << "│" << Color::RESET << "\n";
    }

    std::cout << Color::CYAN << "└" << repeat("─", width - 2) << "┘" << Color::RESET << "\n";
}

void printResult(const std::string& label, const std::string& value, const std::string& extra) {
    std::cout << "\n" << Color::BRIGHT_GREEN << "=> " << Color::BOLD << Color::WHITE << label << ": " 
              << Color::BOLD << Color::BRIGHT_GREEN << value << Color::RESET;
    if (!extra.empty()) {
        std::cout << " " << Color::DIM << "(" << extra << ")" << Color::RESET;
    }
    std::cout << "\n";
}

void printError(const std::string& message) {
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_RED << "[Error] " << Color::RED << message << Color::RESET << "\n";
}

void printSuccess(const std::string& message) {
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_GREEN << "[OK] " << message << Color::RESET << "\n";
}

void printInfo(const std::string& message) {
    std::cout << Color::BRIGHT_CYAN << "[Info] " << Color::CYAN << message << Color::RESET << "\n";
}

void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) {
    if (headers.empty()) return;

    std::vector<size_t> colWidths(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); ++i) {
        colWidths[i] = headers[i].length();
    }
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size() && i < colWidths.size(); ++i) {
            if (row[i].length() > colWidths[i]) {
                colWidths[i] = row[i].length();
            }
        }
    }

    // Top border
    std::cout << Color::CYAN << "┌";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        std::cout << repeat("─", static_cast<int>(colWidths[i]) + 2);
        if (i + 1 < colWidths.size()) std::cout << "┬";
    }
    std::cout << "┐" << Color::RESET << "\n";

    // Headers
    std::cout << Color::CYAN << "│" << Color::RESET;
    for (size_t i = 0; i < headers.size(); ++i) {
        std::cout << " " << Color::BOLD << Color::BRIGHT_WHITE << std::left << std::setw(static_cast<int>(colWidths[i])) << headers[i] << Color::RESET << Color::CYAN << " │" << Color::RESET;
    }
    std::cout << "\n";

    // Divider
    std::cout << Color::CYAN << "├";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        std::cout << repeat("─", static_cast<int>(colWidths[i]) + 2);
        if (i + 1 < colWidths.size()) std::cout << "┼";
    }
    std::cout << "┤" << Color::RESET << "\n";

    // Rows
    for (const auto& row : rows) {
        std::cout << Color::CYAN << "│" << Color::RESET;
        for (size_t i = 0; i < colWidths.size(); ++i) {
            std::string cell = (i < row.size()) ? row[i] : "";
            std::cout << " " << std::left << std::setw(static_cast<int>(colWidths[i])) << cell << Color::CYAN << " │" << Color::RESET;
        }
        std::cout << "\n";
    }

    // Bottom border
    std::cout << Color::CYAN << "└";
    for (size_t i = 0; i < colWidths.size(); ++i) {
        std::cout << repeat("─", static_cast<int>(colWidths[i]) + 2);
        if (i + 1 < colWidths.size()) std::cout << "┴";
    }
    std::cout << "┘" << Color::RESET << "\n";
}

std::string promptString(const std::string& label) {
    std::cout << Color::BOLD << Color::BRIGHT_YELLOW << "> " << label << ": " << Color::RESET;
    std::string input;
    std::getline(std::cin, input);
    size_t start = input.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = input.find_last_not_of(" \t\r\n");
    return input.substr(start, end - start + 1);
}

double promptDouble(const std::string& label, double defaultValue) {
    while (true) {
        std::cout << Color::BOLD << Color::BRIGHT_YELLOW << "> " << label << Color::RESET;
        if (defaultValue != 0.0) {
            std::cout << Color::DIM << " [" << defaultValue << "]" << Color::RESET;
        }
        std::cout << ": ";

        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            return defaultValue;
        }

        try {
            size_t idx;
            double val = std::stod(line, &idx);
            if (idx == line.length() || line.find_first_not_of(" \t\r\n", idx) == std::string::npos) {
                return val;
            }
        } catch (...) {}

        printError("Invalid number format. Try again.");
    }
}

long long promptLong(const std::string& label, long long defaultValue) {
    while (true) {
        std::cout << Color::BOLD << Color::BRIGHT_YELLOW << "> " << label << Color::RESET;
        if (defaultValue != 0) {
            std::cout << Color::DIM << " [" << defaultValue << "]" << Color::RESET;
        }
        std::cout << ": ";

        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            return defaultValue;
        }

        try {
            size_t idx;
            long long val = std::stoll(line, &idx, 0);
            return val;
        } catch (...) {}

        printError("Invalid integer format. Try again.");
    }
}

int promptInt(const std::string& label, int minVal, int maxVal) {
    while (true) {
        std::cout << Color::BOLD << Color::BRIGHT_YELLOW << "> " << label 
                  << Color::DIM << " (" << minVal << "-" << maxVal << "): " << Color::RESET;
        std::string line;
        std::getline(std::cin, line);

        try {
            size_t idx;
            int val = std::stoi(line, &idx);
            if (val >= minVal && val <= maxVal) {
                return val;
            }
            printError("Value out of range (" + std::to_string(minVal) + " to " + std::to_string(maxVal) + ").");
        } catch (...) {
            printError("Please enter a valid integer.");
        }
    }
}

bool promptYesNo(const std::string& label, bool defaultYes) {
    std::cout << Color::BOLD << Color::BRIGHT_YELLOW << "> " << label 
              << Color::DIM << " (" << (defaultYes ? "Y/n" : "y/N") << "): " << Color::RESET;
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return defaultYes;
    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
    return (c == 'y');
}

} // namespace UI
