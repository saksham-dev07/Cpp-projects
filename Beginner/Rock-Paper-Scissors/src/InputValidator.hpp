#pragma once

#include "Types.hpp"
#include <string>

namespace rps {

class InputValidator {
public:
    static int getIntInRange(const std::string& prompt, int minVal, int maxVal);
    static std::string getString(const std::string& prompt, size_t maxLength = 24, const std::string& defaultVal = "Player");
    static bool getConfirmation(const std::string& prompt, bool defaultYes = true);
    static Move getMoveInput(const std::string& prompt, RuleSet ruleSet, bool allowQuit = true);
    static Move getHiddenMoveInput(const std::string& playerName, RuleSet ruleSet);
    static void pause(const std::string& prompt = "Press Enter to continue...");

    static Move parseMoveString(const std::string& input, RuleSet ruleSet);
    static std::string moveToString(Move move);
    static std::string moveToStringWithIcon(Move move);
};

} // namespace rps
