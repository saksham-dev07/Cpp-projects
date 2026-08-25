#include "Terminal.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
#endif

namespace GuessGame {

    ThemeColor Terminal::s_activeTheme = ThemeColor::NeonCyberpunk;
    ColorScheme Terminal::s_colors = {};

    void Terminal::init() {
#ifdef _WIN32
        // Enable ANSI escape sequence processing on Windows Console
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
        // Set console code page to UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
        updateColorScheme();
    }

    void Terminal::setTheme(ThemeColor theme) {
        s_activeTheme = theme;
        updateColorScheme();
    }

    void Terminal::updateColorScheme() {
        const std::string RESET = "\033[0m";
        const std::string BOLD = "\033[1m";

        switch (s_activeTheme) {
            case ThemeColor::NeonCyberpunk:
                s_colors.primary   = "\033[38;5;51m" + BOLD;   // Bright Cyan
                s_colors.secondary = "\033[38;5;201m" + BOLD;  // Neon Magenta
                s_colors.accent    = "\033[38;5;226m" + BOLD;  // Bright Yellow
                s_colors.success   = "\033[38;5;46m" + BOLD;   // Neon Green
                s_colors.warning   = "\033[38;5;214m" + BOLD;  // Orange
                s_colors.error     = "\033[38;5;196m" + BOLD;  // Bright Red
                s_colors.muted     = "\033[38;5;245m";         // Grey
                s_colors.highlight = "\033[38;5;159m" + BOLD;  // Ice Blue
                s_colors.bgCard    = "\033[48;5;236m";         // Dark Slate BG
                s_colors.reset     = RESET;
                break;

            case ThemeColor::RetroEmerald:
                s_colors.primary   = "\033[38;5;48m" + BOLD;   // Emerald Green
                s_colors.secondary = "\033[38;5;35m" + BOLD;   // Forest Green
                s_colors.accent    = "\033[38;5;190m" + BOLD;  // Lime Gold
                s_colors.success   = "\033[38;5;46m" + BOLD;   // Bright Green
                s_colors.warning   = "\033[38;5;220m" + BOLD;  // Amber
                s_colors.error     = "\033[38;5;203m" + BOLD;  // Soft Red
                s_colors.muted     = "\033[38;5;243m";         // Medium Grey
                s_colors.highlight = "\033[38;5;121m" + BOLD;  // Pale Green
                s_colors.bgCard    = "\033[48;5;234m";
                s_colors.reset     = RESET;
                break;

            case ThemeColor::SunsetAmber:
                s_colors.primary   = "\033[38;5;208m" + BOLD;  // Vivid Orange
                s_colors.secondary = "\033[38;5;161m" + BOLD;  // Crimson Pink
                s_colors.accent    = "\033[38;5;220m" + BOLD;  // Golden Yellow
                s_colors.success   = "\033[38;5;48m" + BOLD;   // Mint Green
                s_colors.warning   = "\033[38;5;214m" + BOLD;  // Warm Amber
                s_colors.error     = "\033[38;5;196m" + BOLD;  // Bright Red
                s_colors.muted     = "\033[38;5;242m";         // Warm Grey
                s_colors.highlight = "\033[38;5;224m" + BOLD;  // Peach
                s_colors.bgCard    = "\033[48;5;235m";
                s_colors.reset     = RESET;
                break;

            case ThemeColor::OceanBlue:
                s_colors.primary   = "\033[38;5;39m" + BOLD;   // Sky Blue
                s_colors.secondary = "\033[38;5;33m" + BOLD;   // Deep Azure
                s_colors.accent    = "\033[38;5;86m" + BOLD;   // Turquoise
                s_colors.success   = "\033[38;5;49m" + BOLD;   // Sea Green
                s_colors.warning   = "\033[38;5;221m" + BOLD;  // Sand Yellow
                s_colors.error     = "\033[38;5;204m" + BOLD;  // Coral Red
                s_colors.muted     = "\033[38;5;244m";         // Steel Grey
                s_colors.highlight = "\033[38;5;153m" + BOLD;  // Light Azure
                s_colors.bgCard    = "\033[48;5;235m";
                s_colors.reset     = RESET;
                break;

            case ThemeColor::Monochrome:
            default:
                s_colors.primary   = "\033[1m\033[37m";        // Bold White
                s_colors.secondary = "\033[37m";               // White
                s_colors.accent    = "\033[1m\033[97m";        // Bright White
                s_colors.success   = "\033[1m\033[37m";        // Bold White
                s_colors.warning   = "\033[37m";               // White
                s_colors.error     = "\033[1m\033[91m";        // Red
                s_colors.muted     = "\033[90m";               // Dark Grey
                s_colors.highlight = "\033[7m";                // Inverted
                s_colors.bgCard    = "";
                s_colors.reset     = RESET;
                break;
        }
    }

    void Terminal::clearScreen() {
        std::cout << "\033[2J\033[1;1H" << std::flush;
    }

    void Terminal::pausePrompt() {
        std::cout << "\n" << s_colors.muted << "Press [Enter] to continue..." << s_colors.reset;
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void Terminal::printBanner() {
        std::cout << s_colors.primary << R"(
  ╔═════════════════════════════════════════════════════════════════════════╗
  ║   _  _ _  _ _  _ ___  ____ ____    ____ _  _ ____ ____ ____ ____ ____   ║
  ║   |\ | |  | |\/| |__] |___ |__/    | __ |  | |___ [__  [__  |___ |__/   ║
  ║   | \| |__| |  | |__] |___ |  \    |__] |__| |___ ___] ___] |___ |  \   ║
  ╚═════════════════════════════════════════════════════════════════════════╝
)" << s_colors.reset;
        std::cout << s_colors.secondary << center("★  ULTIMATE C++17 NUMBER GUESSING ARENA  ★", 75) << s_colors.reset << "\n\n";
    }

    void Terminal::printHeader(const std::string& title, const std::string& subtitle) {
        std::cout << "\n" << s_colors.primary << "  ═════════════════════════════════════════════════════════════════════" << s_colors.reset << "\n";
        std::cout << "   " << s_colors.accent << title << s_colors.reset << "\n";
        if (!subtitle.empty()) {
            std::cout << "   " << s_colors.muted << subtitle << s_colors.reset << "\n";
        }
        std::cout << s_colors.primary << "  ═════════════════════════════════════════════════════════════════════" << s_colors.reset << "\n\n";
    }

    void Terminal::printBox(const std::vector<std::string>& lines, const std::string& color) {
        std::string c = color.empty() ? s_colors.primary : color;
        size_t maxLen = 0;
        for (const auto& l : lines) {
            if (l.length() > maxLen) maxLen = l.length();
        }
        maxLen = std::max(maxLen, size_t(50));
        maxLen += 4;

        std::cout << c << "  ┌" << std::string(maxLen, '-') << "┐\n";
        for (const auto& l : lines) {
            std::cout << "  │  " << s_colors.reset << padRight(l, maxLen - 4) << c << "  │\n";
        }
        std::cout << "  └" << std::string(maxLen, '-') << "┘" << s_colors.reset << "\n";
    }

    void Terminal::printProgressBar(int current, int total, int width, const std::string& label) {
        if (total <= 0) total = 1;
        current = std::clamp(current, 0, total);
        double ratio = static_cast<double>(current) / total;
        int filled = static_cast<int>(ratio * width);

        std::cout << "  " << s_colors.muted << label << " [" << s_colors.reset;
        std::cout << s_colors.success << std::string(filled, '#') << s_colors.reset;
        std::cout << s_colors.muted << std::string(width - filled, '.') << s_colors.reset;
        std::cout << s_colors.muted << "] " << s_colors.accent << current << "/" << total 
                  << " (" << static_cast<int>(ratio * 100) << "%)" << s_colors.reset << "\n";
    }

    void Terminal::printProximityIndicator(Proximity prox, int difference) {
        std::cout << "  " << s_colors.muted << "Thermal Radar: " << s_colors.reset;
        switch (prox) {
            case Proximity::Boiling:
                std::cout << "\033[38;5;196m\033[1m[ BOILING HOT! ] (Diff: ±" << difference << ")\033[0m - You are right on target!\n";
                break;
            case Proximity::Burning:
                std::cout << "\033[38;5;208m\033[1m[ BURNING! ] (Diff: ±" << difference << ")\033[0m - Very close!\n";
                break;
            case Proximity::Hot:
                std::cout << "\033[38;5;220m\033[1m[ HOT ] (Diff: ±" << difference << ")\033[0m - Getting warmer.\n";
                break;
            case Proximity::Warm:
                std::cout << "\033[38;5;114m\033[1m[ WARM ] (Diff: ±" << difference << ")\033[0m - In the right ballpark.\n";
                break;
            case Proximity::Cold:
                std::cout << "\033[38;5;39m\033[1m[ COLD ] (Diff: ±" << difference << ")\033[0m - Quite a bit off.\n";
                break;
            case Proximity::Freezing:
            default:
                std::cout << "\033[38;5;27m\033[1m[ FREEZING! ] (Diff: ±" << difference << ")\033[0m - Way off target!\n";
                break;
        }
    }

    void Terminal::printLivesBar(int remaining, int total) {
        std::cout << "  " << s_colors.muted << "Shields / Lives: " << s_colors.reset;
        for (int i = 0; i < total; ++i) {
            if (i < remaining) {
                std::cout << s_colors.error << "♥ " << s_colors.reset;
            } else {
                std::cout << s_colors.muted << "♡ " << s_colors.reset;
            }
        }
        std::cout << s_colors.accent << " [" << remaining << "/" << total << "]" << s_colors.reset << "\n";
    }

    void Terminal::printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) {
        if (headers.empty()) return;
        std::vector<size_t> colWidths(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); ++i) {
            colWidths[i] = std::max(colWidths[i], headers[i].length());
        }
        for (const auto& row : rows) {
            for (size_t i = 0; i < row.size() && i < colWidths.size(); ++i) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }

        // Top border
        std::cout << "  " << s_colors.secondary << "+";
        for (size_t w : colWidths) {
            std::cout << std::string(w + 2, '-') << "+";
        }
        std::cout << s_colors.reset << "\n";

        // Headers
        std::cout << "  " << s_colors.secondary << "|";
        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << " " << s_colors.accent << padRight(headers[i], colWidths[i]) << s_colors.secondary << " |";
        }
        std::cout << s_colors.reset << "\n";

        // Separator
        std::cout << "  " << s_colors.secondary << "+";
        for (size_t w : colWidths) {
            std::cout << std::string(w + 2, '=') << "+";
        }
        std::cout << s_colors.reset << "\n";

        // Rows
        for (const auto& row : rows) {
            std::cout << "  " << s_colors.secondary << "|";
            for (size_t i = 0; i < colWidths.size(); ++i) {
                std::string cell = (i < row.size()) ? row[i] : "";
                std::cout << " " << s_colors.reset << padRight(cell, colWidths[i]) << s_colors.secondary << " |";
            }
            std::cout << s_colors.reset << "\n";
        }

        // Bottom border
        std::cout << "  " << s_colors.secondary << "+";
        for (size_t w : colWidths) {
            std::cout << std::string(w + 2, '-') << "+";
        }
        std::cout << s_colors.reset << "\n";
    }

    void Terminal::beep(bool soundEnabled) {
        if (!soundEnabled) return;
#ifdef _WIN32
        Beep(750, 100);
#else
        std::cout << "\a" << std::flush;
#endif
    }

    void Terminal::playFanfare(bool soundEnabled) {
        if (!soundEnabled) return;
#ifdef _WIN32
        Beep(523, 120); // C5
        Beep(659, 120); // E5
        Beep(784, 120); // G5
        Beep(1046, 250); // C6
#else
        std::cout << "\a" << std::flush;
#endif
    }

    void Terminal::playErrorBeep(bool soundEnabled) {
        if (!soundEnabled) return;
#ifdef _WIN32
        Beep(300, 200);
        Beep(200, 300);
#else
        std::cout << "\a" << std::flush;
#endif
    }

    std::string Terminal::formatScore(int score) {
        std::stringstream ss;
        ss << score << " pts";
        return ss.str();
    }

    std::string Terminal::formatTime(double seconds) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << seconds << "s";
        return ss.str();
    }

    std::string Terminal::padRight(std::string_view text, size_t width) {
        if (text.length() >= width) return std::string(text);
        return std::string(text) + std::string(width - text.length(), ' ');
    }

    std::string Terminal::padLeft(std::string_view text, size_t width) {
        if (text.length() >= width) return std::string(text);
        return std::string(width - text.length(), ' ') + std::string(text);
    }

    std::string Terminal::center(std::string_view text, size_t width) {
        if (text.length() >= width) return std::string(text);
        size_t totalPad = width - text.length();
        size_t leftPad = totalPad / 2;
        size_t rightPad = totalPad - leftPad;
        return std::string(leftPad, ' ') + std::string(text) + std::string(rightPad, ' ');
    }

} // namespace GuessGame
