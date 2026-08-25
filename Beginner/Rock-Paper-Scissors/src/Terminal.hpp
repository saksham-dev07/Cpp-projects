#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace rps {

class Terminal {
public:
    static void init();
    static void clearScreen();
    static void clearLine();
    static void playBeep(bool soundEnabled);
    static void sleepMs(int milliseconds);

    static void setTheme(Theme theme);
    static Theme getTheme();
    static std::string getThemeName(Theme theme);

    // Color styling helpers
    static std::string primary(const std::string& text);
    static std::string secondary(const std::string& text);
    static std::string accent(const std::string& text);
    static std::string success(const std::string& text);
    static std::string warning(const std::string& text);
    static std::string danger(const std::string& text);
    static std::string info(const std::string& text);
    static std::string muted(const std::string& text);
    static std::string bold(const std::string& text);
    static std::string highlight(const std::string& text);

    // Layout components
    static void printHeader(const std::string& title, const std::string& subtitle = "");
    static void printBox(const std::vector<std::string>& lines, int width = 74, bool doubleBorder = true);
    static void printDivider(int width = 74);
    static void printProgressBar(int current, int total, int barWidth = 30, const std::string& label = "");

    // Hidden input for Pass & Play mode
    static std::string readHiddenInput(const std::string& prompt);

private:
    static inline Theme s_theme{Theme::Cyberpunk};
    static std::string colorize(const std::string& text, int colorCode);
};

} // namespace rps
