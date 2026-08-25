#pragma once

#include "Types.hpp"
#include <string>
#include <vector>
#include <string_view>

namespace GuessGame {

    struct ColorScheme {
        std::string primary;
        std::string secondary;
        std::string accent;
        std::string success;
        std::string warning;
        std::string error;
        std::string muted;
        std::string highlight;
        std::string bgCard;
        std::string reset;
    };

    class Terminal {
    public:
        static void init();
        static void setTheme(ThemeColor theme);
        static ThemeColor getTheme() noexcept { return s_activeTheme; }
        static const ColorScheme& colors() noexcept { return s_colors; }

        static void clearScreen();
        static void pausePrompt();
        static void printHeader(const std::string& title, const std::string& subtitle = "");
        static void printBanner();
        static void printBox(const std::vector<std::string>& lines, const std::string& color = "");
        static void printProgressBar(int current, int total, int width = 30, const std::string& label = "");
        static void printProximityIndicator(Proximity prox, int difference);
        static void printLivesBar(int remaining, int total);
        static void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows);
        
        static void beep(bool soundEnabled = true);
        static void playFanfare(bool soundEnabled = true);
        static void playErrorBeep(bool soundEnabled = true);

        static std::string formatScore(int score);
        static std::string formatTime(double seconds);
        static std::string padRight(std::string_view text, size_t width);
        static std::string padLeft(std::string_view text, size_t width);
        static std::string center(std::string_view text, size_t width);

    private:
        static ThemeColor s_activeTheme;
        static ColorScheme s_colors;
        static void updateColorScheme();
    };

} // namespace GuessGame
