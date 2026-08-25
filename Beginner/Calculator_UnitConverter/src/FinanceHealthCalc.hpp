#pragma once

#include <string>

namespace FinanceHealth {

struct EMICalculationResult {
    double monthlyEMI;
    double totalPayment;
    double totalInterest;
    double interestPercentage;
};

struct CompoundInterestResult {
    double principal;
    double futureValue;
    double totalInterest;
    double effectiveRate;
};

struct BMIResult {
    double bmiValue;
    std::string category;
    std::string riskLevel;
    double minHealthyWeightKg;
    double maxHealthyWeightKg;
};

struct BMRResult {
    double bmrCalories;
    double sedentaryCalories;
    double lightActiveCalories;
    double moderateActiveCalories;
    double veryActiveCalories;
};

class FinanceHealthEngine {
public:
    // Loan / Mortgage Monthly EMI calculation
    static EMICalculationResult calculateEMI(double principal, double annualRatePct, int tenureMonths);

    // Simple Interest: (P * R * T) / 100
    static double calculateSimpleInterest(double principal, double ratePct, double timeYears);

    // Compound Interest: A = P * (1 + r/n)^(n*t)
    static CompoundInterestResult calculateCompoundInterest(double principal, double annualRatePct, double timeYears, int compoundFrequencyPerYear);

    // Percentages & Discounts
    static double percentOf(double percentage, double total);
    static double percentageChange(double oldValue, double newValue);
    static double calculateDiscount(double originalPrice, double discountPct, double taxPct = 0.0);

    // Body Mass Index (BMI) & Ideal Weight Range
    static BMIResult calculateBMI(double weightKg, double heightMeters);

    // Basal Metabolic Rate (BMR) via Mifflin-St Jeor equation & TDEE
    static BMRResult calculateBMR(double weightKg, double heightCm, int ageYears, bool isMale);
};

} // namespace FinanceHealth
