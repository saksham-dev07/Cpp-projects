#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "UI.hpp"
#include "Calculator.hpp"
#include "UnitConverter.hpp"
#include "ProgrammerCalc.hpp"
#include "FinanceHealthCalc.hpp"
#include "History.hpp"

// Global session instances
static MathCalc::Calculator g_calc;
static Units::UnitConverter g_converter;
static AppHistory::HistoryManager g_history;

void runCalculatorREPL() {
    UI::clearScreen();
    UI::printHeader("SCIENTIFIC CALCULATOR", "Enter math expressions, 'help' for functions, or 'q' to return");

    std::cout << UI::Color::DIM << " Examples: '2 + 3 * 4', 'sqrt(144) + sin(pi/2)', '2^10', 'fact(6)', '5! + 3'\n" << UI::Color::RESET;

    while (true) {
        std::string expr = UI::promptString("calc");
        if (expr.empty()) continue;
        if (expr == "q" || expr == "exit" || expr == "back" || expr == "menu") {
            break;
        }

        if (expr == "help" || expr == "?") {
            std::cout << "\n" << UI::Color::BOLD << UI::Color::CYAN << "--- Supported Functions & Constants ---" << UI::Color::RESET << "\n";
            std::vector<std::string> headers = {"Function / Constant", "Description"};
            std::vector<std::vector<std::string>> rows;
            for (const auto& f : MathCalc::Calculator::getSupportedFunctions()) {
                rows.push_back({f.first, f.second});
            }
            for (const auto& c : MathCalc::Calculator::getSupportedConstants()) {
                rows.push_back({c.first, c.second});
            }
            UI::printTable(headers, rows);
            continue;
        }

        if (expr == "cls" || expr == "clear") {
            UI::clearScreen();
            UI::printHeader("SCIENTIFIC CALCULATOR", "Enter math expressions, 'help' for functions, or 'q' to return");
            continue;
        }

        try {
            double res = g_calc.evaluate(expr);
            std::string formatted = MathCalc::Calculator::formatResult(res);
            UI::printResult("Result", formatted);
            g_history.addEntry("Math", expr, formatted);
        } catch (const std::exception& ex) {
            UI::printError(ex.what());
        }
    }
}

void runUnitConverterInteractive() {
    while (true) {
        UI::clearScreen();
        UI::printHeader("UNIT CONVERTER", "Select a category or 0 to return");

        const auto& cats = g_converter.getCategories();
        std::vector<std::string> menuLines;
        for (size_t i = 0; i < cats.size(); ++i) {
            std::ostringstream ss;
            ss << "[" << (i + 1) << "] " << cats[i].name << " (" << cats[i].units.size() << " units)";
            menuLines.push_back(ss.str());
        }
        menuLines.push_back("[0] Back to Main Menu");

        UI::printBox("Unit Categories", menuLines);

        int choice = UI::promptInt("Select Category", 0, static_cast<int>(cats.size()));
        if (choice == 0) break;

        const auto& selectedCat = cats[choice - 1];

        while (true) {
            UI::clearScreen();
            UI::printHeader("CONVERT: " + selectedCat.name, "Base: " + selectedCat.baseUnit);

            std::vector<std::string> headers = {"#", "Unit Code", "Unit Name", "Factor to Base"};
            std::vector<std::vector<std::string>> rows;
            for (size_t i = 0; i < selectedCat.units.size(); ++i) {
                const auto& u = selectedCat.units[i];
                std::string factorStr = (selectedCat.category == Units::Category::Temperature) 
                                        ? "Formula" 
                                        : Units::UnitConverter::formatValue(u.factorToBase);
                rows.push_back({std::to_string(i + 1), u.id, u.name, factorStr});
            }
            UI::printTable(headers, rows);

            std::cout << UI::Color::DIM << "Type 'q' to go back.\n" << UI::Color::RESET;

            std::string fromUnit = UI::promptString("Convert from (code/name)");
            if (fromUnit == "q" || fromUnit == "back" || fromUnit.empty()) break;

            std::string toUnit = UI::promptString("Convert to (code/name)");
            if (toUnit == "q" || toUnit == "back" || toUnit.empty()) break;

            double value = UI::promptDouble("Enter value", 1.0);

            try {
                double result = g_converter.convert(value, fromUnit, toUnit, selectedCat.category);
                std::string formatted = Units::UnitConverter::formatValue(result);
                std::string expr = Units::UnitConverter::formatValue(value) + " " + fromUnit + " -> " + toUnit;
                
                UI::printResult("Result", formatted + " " + toUnit);
                g_history.addEntry("Unit", expr, formatted + " " + toUnit);
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
            }

            if (!UI::promptYesNo("Another conversion in this category?", true)) {
                break;
            }
        }
    }
}

void runProgrammerCalcInteractive() {
    ProgrammerCalc::WordSize currentWordSize = ProgrammerCalc::WordSize::Bit64;

    while (true) {
        UI::clearScreen();
        UI::printHeader("PROGRAMMER CALCULATOR", "Radix conversions, bitwise logic & bit inspection");

        std::string wsName = (currentWordSize == ProgrammerCalc::WordSize::Bit64) ? "64-bit (QWORD)" :
                             (currentWordSize == ProgrammerCalc::WordSize::Bit32) ? "32-bit (DWORD)" :
                             (currentWordSize == ProgrammerCalc::WordSize::Bit16) ? "16-bit (WORD)" : "8-bit (BYTE)";

        std::vector<std::string> options = {
            "[1] Convert Number (Hex, Dec, Bin, Oct, ASCII)",
            "[2] Bitwise Operations (AND, OR, XOR, NOT, Shifts, ROL/ROR)",
            "[3] Switch Word Size (Current: " + wsName + ")",
            "[4] Inspect Binary Bit Grid",
            "[0] Back to Main Menu"
        };
        UI::printBox("Programmer Menu", options);

        int choice = UI::promptInt("Select Option", 0, 4);
        if (choice == 0) break;

        if (choice == 1) {
            std::cout << UI::Color::DIM << "Enter value (e.g. 255, 0xFF, 0b11111111, 0o377):\n" << UI::Color::RESET;
            std::string input = UI::promptString("Number");
            if (input.empty()) continue;

            try {
                uint64_t val = ProgrammerCalc::BaseEngine::parseNumber(input);
                auto reps = ProgrammerCalc::BaseEngine::getAllRepresentations(val, currentWordSize);

                std::vector<std::string> headers = {"Radix / Format", "Value"};
                std::vector<std::vector<std::string>> rows = {
                    {"Hexadecimal (HEX)", reps.hex},
                    {"Decimal (Signed)", reps.decSigned},
                    {"Decimal (Unsigned)", reps.decUnsigned},
                    {"Octal (OCT)", reps.oct},
                    {"Binary (BIN)", reps.bin},
                    {"ASCII Characters", reps.ascii},
                    {"Set Bits (PopCount)", std::to_string(reps.popCount)}
                };

                UI::printTable(headers, rows);
                g_history.addEntry("Programmer", input, reps.hex + " | " + reps.bin);
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 2) {
            UI::clearScreen();
            UI::printSection("Bitwise Operations");
            std::cout << "1: AND  2: OR  3: XOR  4: NOT  5: NAND  6: NOR  7: SHL (<<)  8: SHR (>>)  9: ROL  10: ROR\n\n";

            int op = UI::promptInt("Operation", 1, 10);
            std::string inA = UI::promptString("Operand A");
            if (inA.empty()) continue;

            try {
                uint64_t a = ProgrammerCalc::BaseEngine::parseNumber(inA);
                uint64_t res = 0;
                std::string opName;

                if (op == 4) { // Unary NOT
                    res = ProgrammerCalc::BaseEngine::bitNot(a, currentWordSize);
                    opName = "NOT " + inA;
                } else if (op == 7 || op == 8 || op == 9 || op == 10) { // Shift operations
                    int shift = UI::promptInt("Shift count", 0, static_cast<int>(currentWordSize));
                    if (op == 7) {
                        res = ProgrammerCalc::BaseEngine::shiftLeft(a, shift, currentWordSize);
                        opName = inA + " << " + std::to_string(shift);
                    } else if (op == 8) {
                        res = ProgrammerCalc::BaseEngine::shiftRightLogical(a, shift, currentWordSize);
                        opName = inA + " >> " + std::to_string(shift);
                    } else if (op == 9) {
                        res = ProgrammerCalc::BaseEngine::rotateLeft(a, shift, currentWordSize);
                        opName = inA + " ROL " + std::to_string(shift);
                    } else {
                        res = ProgrammerCalc::BaseEngine::rotateRight(a, shift, currentWordSize);
                        opName = inA + " ROR " + std::to_string(shift);
                    }
                } else { // Binary bitwise ops
                    std::string inB = UI::promptString("Operand B");
                    uint64_t b = ProgrammerCalc::BaseEngine::parseNumber(inB);
                    if (op == 1) { res = ProgrammerCalc::BaseEngine::bitAnd(a, b, currentWordSize); opName = inA + " AND " + inB; }
                    else if (op == 2) { res = ProgrammerCalc::BaseEngine::bitOr(a, b, currentWordSize); opName = inA + " OR " + inB; }
                    else if (op == 3) { res = ProgrammerCalc::BaseEngine::bitXor(a, b, currentWordSize); opName = inA + " XOR " + inB; }
                    else if (op == 5) { res = ProgrammerCalc::BaseEngine::bitNand(a, b, currentWordSize); opName = inA + " NAND " + inB; }
                    else if (op == 6) { res = ProgrammerCalc::BaseEngine::bitNor(a, b, currentWordSize); opName = inA + " NOR " + inB; }
                }

                auto reps = ProgrammerCalc::BaseEngine::getAllRepresentations(res, currentWordSize);
                UI::printResult("Result (Hex)", reps.hex);
                UI::printResult("Result (Dec)", reps.decUnsigned);
                UI::printResult("Result (Bin)", reps.bin);

                g_history.addEntry("Programmer", opName, reps.hex + " (" + reps.decUnsigned + ")");
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 3) {
            std::cout << "\n[1] 64-bit (QWORD)\n[2] 32-bit (DWORD)\n[3] 16-bit (WORD)\n[4] 8-bit (BYTE)\n";
            int wsChoice = UI::promptInt("Select Word Size", 1, 4);
            if (wsChoice == 1) currentWordSize = ProgrammerCalc::WordSize::Bit64;
            else if (wsChoice == 2) currentWordSize = ProgrammerCalc::WordSize::Bit32;
            else if (wsChoice == 3) currentWordSize = ProgrammerCalc::WordSize::Bit16;
            else currentWordSize = ProgrammerCalc::WordSize::Bit8;
            UI::printSuccess("Word size updated.");
            UI::pause();
        } else if (choice == 4) {
            std::string input = UI::promptString("Number to inspect");
            try {
                uint64_t val = ProgrammerCalc::BaseEngine::parseNumber(input);
                std::cout << "\n" << UI::Color::BOLD << UI::Color::CYAN << "Bit Layout (" << static_cast<int>(currentWordSize) << "-bit):" << UI::Color::RESET << "\n";
                std::cout << ProgrammerCalc::BaseEngine::getBitGrid(val, currentWordSize) << "\n";
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        }
    }
}

void runFinanceInteractive() {
    while (true) {
        UI::clearScreen();
        UI::printHeader("FINANCIAL CALCULATOR", "Loan EMI, compound interest, discounts & percentages");

        std::vector<std::string> options = {
            "[1] Loan / Mortgage EMI Calculator",
            "[2] Compound Interest Calculator",
            "[3] Simple Interest Calculator",
            "[4] Percentage & Discount Calculator",
            "[0] Back to Main Menu"
        };
        UI::printBox("Financial Options", options);

        int choice = UI::promptInt("Select Option", 0, 4);
        if (choice == 0) break;

        if (choice == 1) {
            UI::clearScreen();
            UI::printSection("Loan EMI Calculator");

            double principal = UI::promptDouble("Loan Principal Amount", 100000);
            double rate = UI::promptDouble("Annual Interest Rate (%)", 8.5);
            int tenureYears = UI::promptInt("Tenure in Years", 1, 40);

            try {
                auto emi = FinanceHealth::FinanceHealthEngine::calculateEMI(principal, rate, tenureYears * 12);
                
                std::vector<std::string> headers = {"Metric", "Value"};
                std::vector<std::vector<std::string>> rows = {
                    {"Monthly EMI", MathCalc::Calculator::formatResult(emi.monthlyEMI, 2)},
                    {"Principal Amount", MathCalc::Calculator::formatResult(principal, 2)},
                    {"Total Interest Payable", MathCalc::Calculator::formatResult(emi.totalInterest, 2)},
                    {"Total Amount Payable", MathCalc::Calculator::formatResult(emi.totalPayment, 2)},
                    {"Interest Ratio", MathCalc::Calculator::formatResult(emi.interestPercentage, 2) + "%"}
                };

                UI::printTable(headers, rows);
                g_history.addEntry("Finance", "EMI: " + std::to_string((int)principal) + " @ " + MathCalc::Calculator::formatResult(rate) + "% (" + std::to_string(tenureYears) + "y)", "EMI: " + MathCalc::Calculator::formatResult(emi.monthlyEMI, 2));
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 2) {
            UI::clearScreen();
            UI::printSection("Compound Interest Calculator");

            double principal = UI::promptDouble("Initial Principal", 10000);
            double rate = UI::promptDouble("Annual Interest Rate (%)", 7.0);
            double years = UI::promptDouble("Duration (Years)", 5.0);
            std::cout << "Compounding: 1: Annually, 2: Semi-Annually, 4: Quarterly, 12: Monthly, 365: Daily\n";
            int freq = UI::promptInt("Frequency per year", 1, 365);

            try {
                auto ci = FinanceHealth::FinanceHealthEngine::calculateCompoundInterest(principal, rate, years, freq);
                
                std::vector<std::string> headers = {"Metric", "Value"};
                std::vector<std::vector<std::string>> rows = {
                    {"Principal Investment", MathCalc::Calculator::formatResult(ci.principal, 2)},
                    {"Future Value (Total)", MathCalc::Calculator::formatResult(ci.futureValue, 2)},
                    {"Total Interest Earned", MathCalc::Calculator::formatResult(ci.totalInterest, 2)},
                    {"Effective Annual Rate (APY)", MathCalc::Calculator::formatResult(ci.effectiveRate, 4) + "%"}
                };

                UI::printTable(headers, rows);
                g_history.addEntry("Finance", "CI: " + std::to_string((int)principal) + " @ " + MathCalc::Calculator::formatResult(rate) + "%", "FV: " + MathCalc::Calculator::formatResult(ci.futureValue, 2));
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 3) {
            double principal = UI::promptDouble("Principal", 5000);
            double rate = UI::promptDouble("Annual Rate (%)", 5.0);
            double years = UI::promptDouble("Time in Years", 3.0);

            try {
                double interest = FinanceHealth::FinanceHealthEngine::calculateSimpleInterest(principal, rate, years);
                UI::printResult("Simple Interest Earned", MathCalc::Calculator::formatResult(interest, 2));
                UI::printResult("Total Maturity Value", MathCalc::Calculator::formatResult(principal + interest, 2));
                g_history.addEntry("Finance", "SI: " + std::to_string((int)principal) + " @ " + MathCalc::Calculator::formatResult(rate) + "%", MathCalc::Calculator::formatResult(interest, 2));
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 4) {
            UI::clearScreen();
            UI::printSection("Percentage & Discounts");
            std::cout << "[1] Calculate X% of Y\n[2] Percentage Change from A to B\n[3] Price Discount & Tax\n";
            int pChoice = UI::promptInt("Select Option", 1, 3);

            if (pChoice == 1) {
                double pct = UI::promptDouble("Percentage (%)", 15.0);
                double total = UI::promptDouble("Total Amount", 200.0);
                double res = FinanceHealth::FinanceHealthEngine::percentOf(pct, total);
                UI::printResult(MathCalc::Calculator::formatResult(pct) + "% of " + MathCalc::Calculator::formatResult(total), MathCalc::Calculator::formatResult(res));
            } else if (pChoice == 2) {
                double oldVal = UI::promptDouble("Original Value (A)", 100.0);
                double newVal = UI::promptDouble("New Value (B)", 125.0);
                try {
                    double change = FinanceHealth::FinanceHealthEngine::percentageChange(oldVal, newVal);
                    std::string sign = (change >= 0) ? "+" : "";
                    UI::printResult("Percentage Change", sign + MathCalc::Calculator::formatResult(change, 2) + "%");
                } catch (const std::exception& ex) {
                    UI::printError(ex.what());
                }
            } else {
                double price = UI::promptDouble("Original Price", 100.0);
                double disc = UI::promptDouble("Discount (%)", 20.0);
                double tax = UI::promptDouble("Sales Tax (%)", 0.0);
                double finalPrice = FinanceHealth::FinanceHealthEngine::calculateDiscount(price, disc, tax);
                UI::printResult("Final Price", MathCalc::Calculator::formatResult(finalPrice, 2));
            }
            UI::pause();
        }
    }
}

void runHealthInteractive() {
    while (true) {
        UI::clearScreen();
        UI::printHeader("HEALTH & FITNESS", "Body Mass Index (BMI) & Basal Metabolic Rate (BMR)");

        std::vector<std::string> options = {
            "[1] Body Mass Index (BMI) Calculator",
            "[2] Basal Metabolic Rate (BMR) & Daily Calories",
            "[0] Back to Main Menu"
        };
        UI::printBox("Health Options", options);

        int choice = UI::promptInt("Select Option", 0, 2);
        if (choice == 0) break;

        if (choice == 1) {
            UI::clearScreen();
            UI::printSection("Body Mass Index (BMI) Calculator");

            bool useMetric = UI::promptYesNo("Use Metric units (kg/cm) [y] or Imperial (lbs/inches) [n]?", true);
            double weightKg = 0;
            double heightM = 0;

            if (useMetric) {
                weightKg = UI::promptDouble("Weight (kg)", 70.0);
                double heightCm = UI::promptDouble("Height (cm)", 175.0);
                heightM = heightCm / 100.0;
            } else {
                double weightLbs = UI::promptDouble("Weight (lbs)", 154.0);
                double heightInches = UI::promptDouble("Height (inches)", 69.0);
                weightKg = weightLbs * 0.45359237;
                heightM = heightInches * 0.0254;
            }

            try {
                auto bmi = FinanceHealth::FinanceHealthEngine::calculateBMI(weightKg, heightM);
                
                std::vector<std::string> headers = {"Metric", "Result"};
                std::vector<std::vector<std::string>> rows = {
                    {"BMI Score", MathCalc::Calculator::formatResult(bmi.bmiValue, 2) + " kg/m²"},
                    {"Category", bmi.category},
                    {"Health Risk", bmi.riskLevel},
                    {"Healthy Weight Range", MathCalc::Calculator::formatResult(bmi.minHealthyWeightKg, 1) + " - " + MathCalc::Calculator::formatResult(bmi.maxHealthyWeightKg, 1) + " kg"}
                };

                UI::printTable(headers, rows);
                g_history.addEntry("Health", "BMI", MathCalc::Calculator::formatResult(bmi.bmiValue, 2) + " (" + bmi.category + ")");
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        } else if (choice == 2) {
            UI::clearScreen();
            UI::printSection("Basal Metabolic Rate (BMR) Calculator");

            double weightKg = UI::promptDouble("Weight (kg)", 70.0);
            double heightCm = UI::promptDouble("Height (cm)", 175.0);
            int age = UI::promptInt("Age (years)", 10, 120);
            bool isMale = UI::promptYesNo("Biological Gender: Male (Y) or Female (N)?", true);

            try {
                auto bmr = FinanceHealth::FinanceHealthEngine::calculateBMR(weightKg, heightCm, age, isMale);
                
                std::vector<std::string> headers = {"Activity Level", "Estimated Daily Calories (TDEE)"};
                std::vector<std::vector<std::string>> rows = {
                    {"Base BMR (Resting)", MathCalc::Calculator::formatResult(bmr.bmrCalories, 0) + " kcal/day"},
                    {"Sedentary (Little/no exercise)", MathCalc::Calculator::formatResult(bmr.sedentaryCalories, 0) + " kcal/day"},
                    {"Lightly Active (1-3 days/week)", MathCalc::Calculator::formatResult(bmr.lightActiveCalories, 0) + " kcal/day"},
                    {"Moderately Active (3-5 days/week)", MathCalc::Calculator::formatResult(bmr.moderateActiveCalories, 0) + " kcal/day"},
                    {"Very Active (6-7 days/week)", MathCalc::Calculator::formatResult(bmr.veryActiveCalories, 0) + " kcal/day"}
                };

                UI::printTable(headers, rows);
                g_history.addEntry("Health", "BMR", MathCalc::Calculator::formatResult(bmr.bmrCalories, 0) + " kcal/day");
                UI::pause();
            } catch (const std::exception& ex) {
                UI::printError(ex.what());
                UI::pause();
            }
        }
    }
}

void showHistoryInteractive() {
    UI::clearScreen();
    UI::printHeader("CALCULATION HISTORY", "View session records, export to file, or clear");

    if (g_history.isEmpty()) {
        UI::printInfo("No calculations recorded yet in this session.");
        UI::pause();
        return;
    }

    std::vector<std::string> headers = {"#", "Time", "Type", "Expression / Input", "Result"};
    std::vector<std::vector<std::string>> rows;
    const auto& entries = g_history.getEntries();
    for (size_t i = 0; i < entries.size(); ++i) {
        rows.push_back({
            std::to_string(i + 1),
            entries[i].timestamp.substr(11),
            entries[i].type,
            entries[i].expression,
            entries[i].result
        });
    }
    UI::printTable(headers, rows);

    std::cout << "\n[1] Export History to File ('history.txt')\n[2] Clear History\n[0] Back\n";
    int choice = UI::promptInt("Select Action", 0, 2);

    if (choice == 1) {
        std::string filename = UI::promptString("Filename (default: history.txt)");
        if (filename.empty()) filename = "history.txt";
        if (g_history.exportToFile(filename)) {
            UI::printSuccess("History saved to '" + filename + "'.");
        } else {
            UI::printError("Failed to write to file '" + filename + "'.");
        }
        UI::pause();
    } else if (choice == 2) {
        if (UI::promptYesNo("Clear all history?", false)) {
            g_history.clear();
            UI::printSuccess("History cleared.");
            UI::pause();
        }
    }
}

void showQuickReference() {
    UI::clearScreen();
    UI::printHeader("QUICK REFERENCE", "Supported math functions, constants, and unit categories");

    std::cout << UI::Color::BOLD << UI::Color::CYAN << "--- Math Functions & Constants ---" << UI::Color::RESET << "\n";
    std::vector<std::string> mathHeaders = {"Item", "Description"};
    std::vector<std::vector<std::string>> mathRows;
    for (const auto& f : MathCalc::Calculator::getSupportedFunctions()) {
        mathRows.push_back({f.first, f.second});
    }
    for (const auto& c : MathCalc::Calculator::getSupportedConstants()) {
        mathRows.push_back({c.first, c.second});
    }
    UI::printTable(mathHeaders, mathRows);

    std::cout << "\n" << UI::Color::BOLD << UI::Color::CYAN << "--- Unit Categories (11 Total) ---" << UI::Color::RESET << "\n";
    std::vector<std::string> catHeaders = {"Category", "Base Unit", "Sample Units"};
    std::vector<std::vector<std::string>> catRows;
    for (const auto& cat : g_converter.getCategories()) {
        std::string sampleUnits;
        for (size_t i = 0; i < cat.units.size() && i < 6; ++i) {
            if (i > 0) sampleUnits += ", ";
            sampleUnits += cat.units[i].id;
        }
        if (cat.units.size() > 6) sampleUnits += ", ...";
        catRows.push_back({cat.name, cat.baseUnit, sampleUnits});
    }
    UI::printTable(catHeaders, catRows);

    UI::pause();
}

void printHelpCli() {
    std::cout << "\nCLI Calculator & Unit Converter\n";
    std::cout << "Usage:\n";
    std::cout << "  calculator                                   # Launch interactive menu mode\n";
    std::cout << "  calculator \"<math_expression>\"               # Direct calculation (e.g. '2^3 + sqrt(16)')\n";
    std::cout << "  calculator convert <val> <from_unit> <to_unit> # Direct unit conversion (e.g. '100 km mi')\n";
    std::cout << "  calculator base <value> <from_radix> <to_radix> # Direct base conversion (e.g. '255 dec hex')\n";
    std::cout << "  calculator --help, -h                        # Show this help guide\n\n";
}

int handleCliArgs(int argc, char* argv[]) {
    std::string command = argv[1];

    if (command == "-h" || command == "--help" || command == "/?" || command == "help") {
        printHelpCli();
        return 0;
    }

    if (command == "convert" || command == "conv") {
        if (argc < 5) {
            UI::printError("Usage: calculator convert <value> <from_unit> <to_unit>");
            return 1;
        }
        try {
            double val = std::stod(argv[2]);
            std::string from = argv[3];
            std::string to = argv[4];
            double res = g_converter.convert(val, from, to);
            std::cout << Units::UnitConverter::formatValue(res) << "\n";
            return 0;
        } catch (const std::exception& ex) {
            UI::printError(ex.what());
            return 1;
        }
    }

    if (command == "base") {
        if (argc < 5) {
            UI::printError("Usage: calculator base <value> <from_radix> <to_radix>");
            return 1;
        }
        try {
            std::string valStr = argv[2];
            auto fromR = ProgrammerCalc::BaseEngine::parseRadixName(argv[3]);
            auto toR = ProgrammerCalc::BaseEngine::parseRadixName(argv[4]);
            std::string res = ProgrammerCalc::BaseEngine::convertBase(valStr, fromR, toR);
            std::cout << res << "\n";
            return 0;
        } catch (const std::exception& ex) {
            UI::printError(ex.what());
            return 1;
        }
    }

    // Default: treat remaining args as math expression
    std::ostringstream exprStream;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) exprStream << " ";
        exprStream << argv[i];
    }
    std::string expr = exprStream.str();

    try {
        double res = g_calc.evaluate(expr);
        std::cout << MathCalc::Calculator::formatResult(res) << "\n";
        return 0;
    } catch (const std::exception& ex) {
        UI::printError(ex.what());
        return 1;
    }
}

int main(int argc, char* argv[]) {
    UI::enableVirtualTerminal();

    if (argc > 1) {
        return handleCliArgs(argc, argv);
    }

    // Main Interactive Loop
    while (true) {
        UI::clearScreen();
        UI::printHeader("CLI CALCULATOR & UNIT CONVERTER", "Engineering, Math & Conversion Suite");

        std::vector<std::string> menuOptions = {
            "[1] Scientific & Arithmetic Calculator (REPL)",
            "[2] Multi-Category Unit Converter (11 Categories)",
            "[3] Programmer / Radix Converter & Bitwise Engine",
            "[4] Financial & Loan EMI Calculator",
            "[5] Health & Fitness (BMI / BMR) Calculator",
            "[6] Session Calculation History (" + std::to_string(g_history.size()) + " records)",
            "[7] Quick Reference & Supported Functions",
            "[0] Exit"
        };

        UI::printBox("Main Menu", menuOptions);

        int choice = UI::promptInt("Select Option", 0, 7);

        switch (choice) {
            case 1: runCalculatorREPL(); break;
            case 2: runUnitConverterInteractive(); break;
            case 3: runProgrammerCalcInteractive(); break;
            case 4: runFinanceInteractive(); break;
            case 5: runHealthInteractive(); break;
            case 6: showHistoryInteractive(); break;
            case 7: showQuickReference(); break;
            case 0:
                UI::clearScreen();
                std::cout << UI::Color::BOLD << UI::Color::BRIGHT_GREEN << "\nExiting calculator. Goodbye!\n\n" << UI::Color::RESET;
                return 0;
        }
    }

    return 0;
}
