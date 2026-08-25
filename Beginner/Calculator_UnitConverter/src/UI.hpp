#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace UI {

    void enableVirtualTerminal();

    namespace Color {
        inline const std::string RESET         = "\033[0m";
        inline const std::string BOLD          = "\033[1m";
        inline const std::string DIM           = "\033[2m";
        inline const std::string ITALIC        = "\033[3m";
        inline const std::string UNDERLINE     = "\033[4m";

        inline const std::string RED           = "\033[31m";
        inline const std::string GREEN         = "\033[32m";
        inline const std::string YELLOW        = "\033[33m";
        inline const std::string BLUE          = "\033[34m";
        inline const std::string MAGENTA       = "\033[35m";
        inline const std::string CYAN          = "\033[36m";
        inline const std::string WHITE         = "\033[37m";

        inline const std::string BRIGHT_RED    = "\033[91m";
        inline const std::string BRIGHT_GREEN  = "\033[92m";
        inline const std::string BRIGHT_YELLOW = "\033[93m";
        inline const std::string BRIGHT_CYAN   = "\033[96m";
        inline const std::string BRIGHT_WHITE  = "\033[97m";
    }

    void clearScreen();
    void pause(const std::string& prompt = "Press Enter to continue...");

    void printHeader(const std::string& title, const std::string& subtitle = "");
    void printSection(const std::string& title);
    void printBox(const std::string& title, const std::vector<std::string>& lines, int minWidth = 58);
    void printResult(const std::string& label, const std::string& value, const std::string& extra = "");
    void printError(const std::string& message);
    void printSuccess(const std::string& message);
    void printInfo(const std::string& message);
    void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows);

    // Interactive input helpers
    std::string promptString(const std::string& label);
    double promptDouble(const std::string& label, double defaultValue = 0.0);
    long long promptLong(const std::string& label, long long defaultValue = 0);
    int promptInt(const std::string& label, int minVal, int maxVal);
    bool promptYesNo(const std::string& label, bool defaultYes = true);
}
