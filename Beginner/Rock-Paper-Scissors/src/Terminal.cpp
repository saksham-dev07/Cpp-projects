#include "Terminal.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace rps {

namespace {
    constexpr const char* Reset = "\033[0m";
    constexpr const char* Bold  = "\033[1m";

    struct Palette {
        int primary;
        int secondary;
        int accent;
        int success;
        int warning;
        int danger;
        int info;
        int muted;
    };

    // Color definitions for each theme
    constexpr Palette kPalettes[] = {
        // Cyberpunk
        { 51,  201, 226, 48,  214, 196, 75,  243 },
        // Emerald
        { 46,  118, 190, 82,  220, 160, 50,  240 },
        // Sunset
        { 208, 197, 220, 119, 215, 160, 216, 242 },
        // Ocean
        { 39,  45,  159, 49,  221, 203, 117, 241 },
        // Amethyst
        { 141, 177, 219, 120, 222, 198, 183, 242 },
        // Monochrome
        { 255, 250, 229, 121, 222, 203, 252, 242 }
    };

    inline const Palette& currentPalette() {
        size_t idx = static_cast<size_t>(Terminal::getTheme());
        if (idx >= sizeof(kPalettes) / sizeof(kPalettes[0])) {
            idx = 0;
        }
        return kPalettes[idx];
    }
}

void Terminal::init() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif
}

void Terminal::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void Terminal::clearLine() {
    std::cout << "\033[2K\r" << std::flush;
}

void Terminal::playBeep(bool soundEnabled) {
    if (soundEnabled) {
        std::cout << "\a" << std::flush;
    }
}

void Terminal::sleepMs(int milliseconds) {
    if (milliseconds > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}

void Terminal::setTheme(Theme theme) {
    s_theme = theme;
}

Theme Terminal::getTheme() {
    return s_theme;
}

std::string Terminal::getThemeName(Theme theme) {
    switch (theme) {
        case Theme::Cyberpunk:  return "Neon Cyberpunk (Cyan / Magenta)";
        case Theme::Emerald:    return "Matrix Emerald (Green / Lime)";
        case Theme::Sunset:     return "Sunset Crimson (Amber / Gold)";
        case Theme::Ocean:      return "Deep Ocean (Azure / Sky Blue)";
        case Theme::Amethyst:   return "Royal Amethyst (Purple / Rose)";
        case Theme::Monochrome: return "Monochrome Clean (Slate / Silver)";
        default:                return "Cyberpunk";
    }
}

std::string Terminal::colorize(const std::string& text, int colorCode) {
    return "\033[38;5;" + std::to_string(colorCode) + "m" + text + Reset;
}

std::string Terminal::primary(const std::string& text) {
    return colorize(text, currentPalette().primary);
}

std::string Terminal::secondary(const std::string& text) {
    return colorize(text, currentPalette().secondary);
}

std::string Terminal::accent(const std::string& text) {
    return colorize(text, currentPalette().accent);
}

std::string Terminal::success(const std::string& text) {
    return colorize(text, currentPalette().success);
}

std::string Terminal::warning(const std::string& text) {
    return colorize(text, currentPalette().warning);
}

std::string Terminal::danger(const std::string& text) {
    return colorize(text, currentPalette().danger);
}

std::string Terminal::info(const std::string& text) {
    return colorize(text, currentPalette().info);
}

std::string Terminal::muted(const std::string& text) {
    return colorize(text, currentPalette().muted);
}

std::string Terminal::bold(const std::string& text) {
    return std::string(Bold) + text + Reset;
}

std::string Terminal::highlight(const std::string& text) {
    return std::string(Bold) + accent(text);
}

void Terminal::printHeader(const std::string& title, const std::string& subtitle) {
    std::cout << "\n";
    std::cout << primary("  ╔═════════════════════════════════════════════════════════════════════════╗\n");
    
    int padTotal = 73 - static_cast<int>(title.length());
    int padLeft = padTotal > 0 ? padTotal / 2 : 0;
    int padRight = padTotal > 0 ? (padTotal - padLeft) : 0;
    
    std::cout << primary("  ║") << std::string(padLeft, ' ') 
              << bold(secondary(title)) 
              << std::string(padRight, ' ') << primary("║\n");

    if (!subtitle.empty()) {
        int subPadTotal = 73 - static_cast<int>(subtitle.length());
        int subPadLeft = subPadTotal > 0 ? subPadTotal / 2 : 0;
        int subPadRight = subPadTotal > 0 ? (subPadTotal - subPadLeft) : 0;
        std::cout << primary("  ║") << std::string(subPadLeft, ' ') 
                  << muted(subtitle) 
                  << std::string(subPadRight, ' ') << primary("║\n");
    }

    std::cout << primary("  ╚═════════════════════════════════════════════════════════════════════════╝\n");
}

void Terminal::printBox(const std::vector<std::string>& lines, int width, bool doubleBorder) {
    std::string topLeft  = doubleBorder ? "╔" : "┌";
    std::string topRight = doubleBorder ? "╗" : "┐";
    std::string botLeft  = doubleBorder ? "╚" : "└";
    std::string botRight = doubleBorder ? "╝" : "┘";
    std::string horiz    = doubleBorder ? "═" : "─";
    std::string vert     = doubleBorder ? "║" : "│";

    std::string hLine;
    for (int i = 0; i < width - 2; ++i) hLine += horiz;

    std::cout << primary("  " + topLeft + hLine + topRight) << "\n";
    for (const auto& line : lines) {
        std::cout << primary("  " + vert) << " " << line << "\n";
    }
    std::cout << primary("  " + botLeft + hLine + botRight) << "\n";
}

void Terminal::printDivider(int width) {
    std::string line(width, '-');
    std::cout << muted("  " + line) << "\n";
}

void Terminal::printProgressBar(int current, int total, int barWidth, const std::string& label) {
    if (total <= 0) total = 1;
    current = std::max(0, std::min(current, total));

    float fraction = static_cast<float>(current) / static_cast<float>(total);
    int filled = static_cast<int>(fraction * barWidth);

    std::cout << "  " << primary(label) << " [";
    std::cout << success(std::string(filled, '#'));
    std::cout << muted(std::string(barWidth - filled, '.'));
    std::cout << "] " << static_cast<int>(fraction * 100.0f) << "% (" 
              << current << "/" << total << ")\n";
}

std::string Terminal::readHiddenInput(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string input;

#ifdef _WIN32
    while (true) {
        char ch = static_cast<char>(_getch());
        if (ch == '\r' || ch == '\n') {
            std::cout << "\n";
            break;
        }
        if (ch == '\b') {
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        } else if (ch >= 32 && ch <= 126) {
            input.push_back(ch);
            std::cout << "*" << std::flush;
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::getline(std::cin, input);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
#endif

    return input;
}

} // namespace rps
