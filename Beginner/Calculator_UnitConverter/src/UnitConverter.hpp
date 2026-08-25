#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace Units {

class ConversionError : public std::runtime_error {
public:
    explicit ConversionError(const std::string& msg) : std::runtime_error(msg) {}
};

enum class Category {
    Length,
    Mass,
    Temperature,
    DataStorage,
    Time,
    Speed,
    Area,
    Volume,
    Pressure,
    Energy,
    Power
};

struct UnitInfo {
    std::string id;          // standard short symbol (e.g. "km")
    std::string name;        // display name (e.g. "Kilometers")
    double factorToBase;     // multiplier to convert to base unit
    std::vector<std::string> aliases; // alternative spellings & abbreviations
};

struct CategoryInfo {
    Category category;
    std::string name;
    std::string baseUnit;
    std::vector<UnitInfo> units;
};

class UnitConverter {
public:
    UnitConverter();

    // Auto-detects category from unit names and converts
    double convert(double value, const std::string& fromUnit, const std::string& toUnit);

    // Converts within a specified category
    double convert(double value, const std::string& fromUnit, const std::string& toUnit, Category category);

    bool findCategoryForUnit(const std::string& unitStr, Category& outCategory) const;
    const std::vector<CategoryInfo>& getCategories() const;
    const CategoryInfo& getCategoryInfo(Category cat) const;
    static std::string getCategoryName(Category cat);
    static std::string formatValue(double value, int precision = 6);

private:
    std::vector<CategoryInfo> categories;
    std::unordered_map<std::string, std::pair<Category, size_t>> unitLookup;

    void initCategories();
    void buildLookup();
    std::string sanitizeUnit(const std::string& str) const;
    double convertTemperature(double value, const std::string& fromUnit, const std::string& toUnit) const;
};

} // namespace Units
