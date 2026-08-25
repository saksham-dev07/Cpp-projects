#pragma once

#include <string>
#include <string_view>

namespace GuessGame {

    class InputValidator {
    public:
        static int getInt(const std::string& prompt, int minVal, int maxVal);
        static int getIntWithSpecial(const std::string& prompt, int minVal, int maxVal, int specialVal, const std::string& specialHelp = "");
        static std::string getBullsCowsGuess(const std::string& prompt);
        static char getChar(const std::string& prompt, std::string_view validChars);
        static std::string getString(const std::string& prompt, bool allowEmpty = false);
        static bool getYesNo(const std::string& prompt, bool defaultYes = true);
        static void flushInput();
    };

} // namespace GuessGame
