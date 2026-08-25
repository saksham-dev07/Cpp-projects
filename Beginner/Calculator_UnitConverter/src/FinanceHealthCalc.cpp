#include "FinanceHealthCalc.hpp"
#include <cmath>
#include <stdexcept>

namespace FinanceHealth {

EMICalculationResult FinanceHealthEngine::calculateEMI(double principal, double annualRatePct, int tenureMonths) {
    if (principal <= 0 || tenureMonths <= 0) {
        throw std::runtime_error("Principal and tenure must be positive values.");
    }

    EMICalculationResult result{};
    if (annualRatePct <= 0) {
        result.monthlyEMI = principal / tenureMonths;
        result.totalPayment = principal;
        result.totalInterest = 0.0;
        result.interestPercentage = 0.0;
        return result;
    }

    double monthlyRate = (annualRatePct / 12.0) / 100.0;
    double compound = std::pow(1.0 + monthlyRate, tenureMonths);
    result.monthlyEMI = (principal * monthlyRate * compound) / (compound - 1.0);
    result.totalPayment = result.monthlyEMI * tenureMonths;
    result.totalInterest = result.totalPayment - principal;
    result.interestPercentage = (result.totalInterest / result.totalPayment) * 100.0;
    return result;
}

double FinanceHealthEngine::calculateSimpleInterest(double principal, double ratePct, double timeYears) {
    if (principal < 0 || timeYears < 0) {
        throw std::runtime_error("Principal and time cannot be negative.");
    }
    return (principal * ratePct * timeYears) / 100.0;
}

CompoundInterestResult FinanceHealthEngine::calculateCompoundInterest(double principal, double annualRatePct, double timeYears, int compoundFrequencyPerYear) {
    if (principal < 0 || timeYears < 0 || compoundFrequencyPerYear <= 0) {
        throw std::runtime_error("Invalid parameters for compound interest.");
    }

    CompoundInterestResult result{};
    result.principal = principal;
    double r = (annualRatePct / 100.0);
    int n = compoundFrequencyPerYear;
    double nt = n * timeYears;

    result.futureValue = principal * std::pow(1.0 + (r / n), nt);
    result.totalInterest = result.futureValue - principal;
    result.effectiveRate = (std::pow(1.0 + (r / n), n) - 1.0) * 100.0;
    return result;
}

double FinanceHealthEngine::percentOf(double percentage, double total) {
    return (percentage / 100.0) * total;
}

double FinanceHealthEngine::percentageChange(double oldValue, double newValue) {
    if (oldValue == 0.0) {
        throw std::runtime_error("Original value cannot be zero for percentage change.");
    }
    return ((newValue - oldValue) / std::abs(oldValue)) * 100.0;
}

double FinanceHealthEngine::calculateDiscount(double originalPrice, double discountPct, double taxPct) {
    if (originalPrice < 0 || discountPct < 0) {
        throw std::runtime_error("Price and discount must be non-negative.");
    }
    double discounted = originalPrice * (1.0 - discountPct / 100.0);
    if (taxPct > 0) {
        discounted += discounted * (taxPct / 100.0);
    }
    return discounted;
}

BMIResult FinanceHealthEngine::calculateBMI(double weightKg, double heightMeters) {
    if (weightKg <= 0 || heightMeters <= 0) {
        throw std::runtime_error("Weight and height must be positive.");
    }

    BMIResult res{};
    res.bmiValue = weightKg / (heightMeters * heightMeters);

    if (res.bmiValue < 18.5) {
        res.category = "Underweight";
        res.riskLevel = "Higher risk of deficiency / malnutrition";
    } else if (res.bmiValue < 25.0) {
        res.category = "Normal / Healthy Weight";
        res.riskLevel = "Low risk";
    } else if (res.bmiValue < 30.0) {
        res.category = "Overweight";
        res.riskLevel = "Moderate health risk";
    } else if (res.bmiValue < 35.0) {
        res.category = "Obesity Class I";
        res.riskLevel = "High risk";
    } else if (res.bmiValue < 40.0) {
        res.category = "Obesity Class II";
        res.riskLevel = "Very high risk";
    } else {
        res.category = "Obesity Class III (Severe)";
        res.riskLevel = "Extremely high risk";
    }

    // Normal range benchmark: 18.5 to 24.9 BMI
    res.minHealthyWeightKg = 18.5 * heightMeters * heightMeters;
    res.maxHealthyWeightKg = 24.9 * heightMeters * heightMeters;
    return res;
}

BMRResult FinanceHealthEngine::calculateBMR(double weightKg, double heightCm, int ageYears, bool isMale) {
    if (weightKg <= 0 || heightCm <= 0 || ageYears <= 0) {
        throw std::runtime_error("Weight, height, and age must be positive.");
    }

    // Mifflin-St Jeor equation
    double genderOffset = isMale ? 5.0 : -161.0;
    double baseBmr = (10.0 * weightKg) + (6.25 * heightCm) - (5.0 * ageYears) + genderOffset;

    BMRResult res{};
    res.bmrCalories = baseBmr;
    res.sedentaryCalories = baseBmr * 1.2;
    res.lightActiveCalories = baseBmr * 1.375;
    res.moderateActiveCalories = baseBmr * 1.55;
    res.veryActiveCalories = baseBmr * 1.725;
    return res;
}

} // namespace FinanceHealth
