#include "UnitConverter.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Units {

UnitConverter::UnitConverter() {
    initCategories();
    buildLookup();
}

std::string UnitConverter::sanitizeUnit(const std::string& str) const {
    std::string s;
    for (char c : str) {
        if (c != ' ' && c != '_' && c != '-') {
            s += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    return s;
}

void UnitConverter::initCategories() {
    // 1. Length & Distance (Base: meter)
    categories.push_back({
        Category::Length,
        "Length & Distance",
        "m",
        {
            {"m",   "Meters",              1.0,                 {"meter", "meters", "metre", "metres"}},
            {"km",  "Kilometers",          1000.0,              {"kilometer", "kilometers", "kilometre"}},
            {"cm",  "Centimeters",         0.01,                {"centimeter", "centimeters"}},
            {"mm",  "Millimeters",         0.001,               {"millimeter", "millimeters"}},
            {"um",  "Micrometers",         1e-6,                {"micrometer", "micrometers", "micron", "microns"}},
            {"nm",  "Nanometers",          1e-9,                {"nanometer", "nanometers"}},
            {"mi",  "Miles",               1609.344,            {"mile", "miles"}},
            {"yd",  "Yards",               0.9144,              {"yard", "yards"}},
            {"ft",  "Feet",                0.3048,              {"foot", "feet"}},
            {"in",  "Inches",              0.0254,              {"inch", "inches"}},
            {"nmi", "Nautical Miles",      1852.0,              {"nauticalmile", "nauticalmiles"}},
            {"ly",  "Light Years",         9.4607304725808e15,  {"lightyear", "lightyears"}},
            {"au",  "Astronomical Units",  1.495978707e11,      {"astronomicalunit"}}
        }
    });

    // 2. Mass & Weight (Base: kilogram)
    categories.push_back({
        Category::Mass,
        "Mass & Weight",
        "kg",
        {
            {"kg",     "Kilograms",          1.0,           {"kilogram", "kilograms", "kilo", "kilos"}},
            {"g",      "Grams",              0.001,         {"gram", "grams", "gm"}},
            {"mg",     "Milligrams",         1e-6,          {"milligram", "milligrams"}},
            {"ug",     "Micrograms",         1e-9,          {"microgram", "micrograms", "mcg"}},
            {"t",      "Metric Tons",        1000.0,        {"tonne", "tonnes", "metricton"}},
            {"lb",     "Pounds",             0.45359237,    {"pound", "pounds", "lbs"}},
            {"oz",     "Ounces",             0.028349523125,{"ounce", "ounces"}},
            {"stone",  "Stones",             6.35029318,    {"st", "stones"}},
            {"ct",     "Carats",             0.0002,        {"carat", "carats"}},
            {"ton_us", "US Short Tons",      907.18474,     {"shortton", "uston"}},
            {"ton_uk", "Imperial Long Tons", 1016.0469088,  {"longton", "ukton"}}
        }
    });

    // 3. Temperature (Special non-linear conversion)
    categories.push_back({
        Category::Temperature,
        "Temperature",
        "K",
        {
            {"c", "Celsius",    1.0, {"celsius", "centigrade", "degc", "°c"}},
            {"f", "Fahrenheit", 1.0, {"fahrenheit", "degf", "°f"}},
            {"k", "Kelvin",     1.0, {"kelvin", "degk"}},
            {"r", "Rankine",    1.0, {"rankine", "degr", "°r"}}
        }
    });

    // 4. Digital Data Storage (Base: Byte)
    categories.push_back({
        Category::DataStorage,
        "Data Storage",
        "B",
        {
            {"bit",  "Bits",                  0.125,                    {"bits"}},
            {"b",    "Bytes",                 1.0,                      {"byte", "bytes"}},
            {"kb",   "Kilobytes (Decimal)",   1000.0,                   {"kilobyte", "kilobytes"}},
            {"mb",   "Megabytes (Decimal)",   1e6,                      {"megabyte", "megabytes"}},
            {"gb",   "Gigabytes (Decimal)",   1e9,                      {"gigabyte", "gigabytes"}},
            {"tb",   "Terabytes (Decimal)",   1e12,                     {"terabyte", "terabytes"}},
            {"pb",   "Petabytes (Decimal)",   1e15,                     {"petabyte", "petabytes"}},
            {"kib",  "Kibibytes (Binary)",    1024.0,                   {"kibibyte", "kibibytes"}},
            {"mib",  "Mebibytes (Binary)",    1048576.0,                {"mebibyte", "mebibytes"}},
            {"gib",  "Gibibytes (Binary)",    1073741824.0,             {"gibibyte", "gibibytes"}},
            {"tib",  "Tebibytes (Binary)",    1099511627776.0,          {"tebibyte", "tebibytes"}},
            {"pib",  "Pebibytes (Binary)",    1125899906842624.0,       {"pebibyte", "pebibytes"}},
            {"kbit", "Kilobits",              125.0,                    {"kilobit", "kilobits"}},
            {"mbit", "Megabits",              125000.0,                 {"megabit", "megabits"}},
            {"gbit", "Gigabits",              125000000.0,              {"gigabit", "gigabits"}}
        }
    });

    // 5. Time (Base: second)
    categories.push_back({
        Category::Time,
        "Time",
        "s",
        {
            {"ns",      "Nanoseconds",                1e-9,        {"nanosecond", "nanoseconds"}},
            {"us",      "Microseconds",               1e-6,        {"microsecond", "microseconds"}},
            {"ms",      "Milliseconds",              0.001,       {"millisecond", "milliseconds"}},
            {"s",       "Seconds",                   1.0,         {"second", "seconds", "sec", "secs"}},
            {"min",     "Minutes",                   60.0,        {"minute", "minutes"}},
            {"h",       "Hours",                     3600.0,      {"hour", "hours", "hr", "hrs"}},
            {"d",       "Days",                      86400.0,     {"day", "days"}},
            {"week",    "Weeks",                     604800.0,    {"wk", "wks", "weeks"}},
            {"month",   "Months (avg 30.44 days)",   2629800.0,   {"mo", "months"}},
            {"yr",      "Years (avg 365.25 days)",   31557600.0,  {"year", "years", "yrs"}},
            {"decade",  "Decades",                   315576000.0, {"decades"}},
            {"century", "Centuries",                 3155760000.0,{"centuries"}}
        }
    });

    // 6. Speed & Velocity (Base: m/s)
    categories.push_back({
        Category::Speed,
        "Speed & Velocity",
        "m/s",
        {
            {"m/s",     "Meters per second",         1.0,         {"mps", "meterpersecond"}},
            {"km/h",    "Kilometers per hour",       1.0 / 3.6,   {"kph", "kmh", "kilometerperhour"}},
            {"mph",     "Miles per hour",            0.44704,     {"mi/h", "mileperhour"}},
            {"knot",    "Knots",                     1852.0/3600.0,{"knots", "kt", "kts"}},
            {"ft/s",    "Feet per second",           0.3048,      {"fps", "footpersecond"}},
            {"mach",    "Mach (sea level)",          343.0,       {"speedofsound"}},
            {"c_light", "Speed of Light in vacuum",  299792458.0, {"lightspeed", "speedoflight"}}
        }
    });

    // 7. Area (Base: m²)
    categories.push_back({
        Category::Area,
        "Area",
        "m2",
        {
            {"m2",   "Square Meters",      1.0,            {"sqm", "sqmeter", "m^2"}},
            {"km2",  "Square Kilometers",  1e6,            {"sqkm", "sqkilometer", "km^2"}},
            {"cm2",  "Square Centimeters", 1e-4,           {"sqcm", "cm^2"}},
            {"mm2",  "Square Millimeters", 1e-6,           {"sqmm", "mm^2"}},
            {"ft2",  "Square Feet",        0.09290304,     {"sqft", "sqfoot", "ft^2"}},
            {"in2",  "Square Inches",      0.00064516,     {"sqin", "sqinch", "in^2"}},
            {"yd2",  "Square Yards",       0.83612736,     {"sqyd", "sqyard", "yd^2"}},
            {"mi2",  "Square Miles",       2589988.110336, {"sqmi", "sqmile", "mi^2"}},
            {"acre", "Acres",              4046.8564224,   {"acres"}},
            {"ha",   "Hectares",           10000.0,        {"hectare", "hectares"}}
        }
    });

    // 8. Volume & Capacity (Base: Liter)
    categories.push_back({
        Category::Volume,
        "Volume & Capacity",
        "L",
        {
            {"l",      "Liters",               1.0,            {"liter", "liters", "litre"}},
            {"ml",     "Milliliters",          0.001,          {"milliliter", "milliliters", "cc"}},
            {"m3",     "Cubic Meters",         1000.0,         {"cum", "cubicmeter", "m^3"}},
            {"cm3",    "Cubic Centimeters",    0.001,          {"cubiccentimeter", "cm^3"}},
            {"gal",    "Gallons (US)",         3.785411784,    {"gallon", "gallons", "usgal"}},
            {"gal_uk", "Imperial Gallons",     4.54609,        {"ukgal", "imperialgallon"}},
            {"qt",     "Quarts (US)",          0.946352946,    {"quart", "quarts"}},
            {"pt",     "Pints (US)",           0.473176473,    {"pint", "pints"}},
            {"cup",    "Cups (US)",            0.2365882365,   {"cups"}},
            {"fl_oz",  "Fluid Ounces (US)",    0.0295735295625,{"floz", "fluidounce"}},
            {"tbsp",   "Tablespoons (US)",     0.01478676478125,{"tablespoon"}},
            {"tsp",    "Teaspoons (US)",       0.00492892159375,{"teaspoon"}},
            {"ft3",    "Cubic Feet",           28.316846592,   {"cuft", "ft^3"}},
            {"in3",    "Cubic Inches",         0.016387064,    {"cuin", "in^3"}}
        }
    });

    // 9. Pressure (Base: Pascal)
    categories.push_back({
        Category::Pressure,
        "Pressure",
        "Pa",
        {
            {"pa",   "Pascals",                  1.0,           {"pascal", "pascals"}},
            {"kpa",  "Kilopascals",              1000.0,        {"kilopascal"}},
            {"mpa",  "Megapascals",              1e6,           {"megapascal"}},
            {"bar",  "Bars",                     100000.0,      {"bars"}},
            {"mbar", "Millibars",                100.0,         {"millibar", "hpa"}},
            {"psi",  "Pounds per Sq Inch (psi)", 6894.757293168,{"psig"}},
            {"atm",  "Standard Atmospheres",     101325.0,      {"atmosphere", "atmospheres"}},
            {"mmhg", "Torr / mmHg",              133.322387415, {"torr"}},
            {"inhg", "Inches of Mercury",        3386.389,      {"inchhg"}}
        }
    });

    // 10. Energy (Base: Joule)
    categories.push_back({
        Category::Energy,
        "Energy",
        "J",
        {
            {"j",    "Joules",                    1.0,           {"joule", "joules"}},
            {"kj",   "Kilojoules",                1000.0,        {"kilojoule"}},
            {"mj",   "Megajoules",                1e6,           {"megajoule"}},
            {"cal",  "Calories",                  4.184,         {"calorie", "calories"}},
            {"kcal", "Kilocalories (Food kcal)",  4184.0,        {"kilocalorie", "foodcal"}},
            {"wh",   "Watt-hours",                3600.0,        {"watthour"}},
            {"kwh",  "Kilowatt-hours",            3.6e6,         {"kilowatthour"}},
            {"btu",  "BTU (British Thermal)",     1055.05585262, {"btus"}},
            {"ev",   "Electronvolts",             1.602176634e-19,{"electronvolt"}}
        }
    });

    // 11. Power (Base: Watt)
    categories.push_back({
        Category::Power,
        "Power",
        "W",
        {
            {"w",    "Watts",                  1.0,                 {"watt", "watts"}},
            {"kw",   "Kilowatts",              1000.0,              {"kilowatt"}},
            {"mw",   "Megawatts",              1e6,                 {"megawatt"}},
            {"hp",   "Horsepower (Mechanical)",745.69987158227022,  {"horsepower"}},
            {"cals", "Calories per sec",       4.184,               {"cal/s"}},
            {"btuh", "BTU per hour",           0.29307107,          {"btu/h"}}
        }
    });
}

void UnitConverter::buildLookup() {
    unitLookup.clear();
    for (const auto& cat : categories) {
        for (size_t uIdx = 0; uIdx < cat.units.size(); ++uIdx) {
            const auto& unit = cat.units[uIdx];
            unitLookup[sanitizeUnit(unit.id)] = {cat.category, uIdx};
            unitLookup[sanitizeUnit(unit.name)] = {cat.category, uIdx};
            for (const auto& alias : unit.aliases) {
                unitLookup[sanitizeUnit(alias)] = {cat.category, uIdx};
            }
        }
    }
}

bool UnitConverter::findCategoryForUnit(const std::string& unitStr, Category& outCategory) const {
    std::string key = sanitizeUnit(unitStr);
    auto it = unitLookup.find(key);
    if (it != unitLookup.end()) {
        outCategory = it->second.first;
        return true;
    }
    return false;
}

const std::vector<CategoryInfo>& UnitConverter::getCategories() const {
    return categories;
}

const CategoryInfo& UnitConverter::getCategoryInfo(Category cat) const {
    for (const auto& ci : categories) {
        if (ci.category == cat) return ci;
    }
    throw ConversionError("Category not found");
}

std::string UnitConverter::getCategoryName(Category cat) {
    switch (cat) {
        case Category::Length:      return "Length & Distance";
        case Category::Mass:        return "Mass & Weight";
        case Category::Temperature: return "Temperature";
        case Category::DataStorage: return "Data Storage";
        case Category::Time:        return "Time";
        case Category::Speed:       return "Speed & Velocity";
        case Category::Area:        return "Area";
        case Category::Volume:      return "Volume & Capacity";
        case Category::Pressure:    return "Pressure";
        case Category::Energy:      return "Energy";
        case Category::Power:       return "Power";
    }
    return "Unknown";
}

double UnitConverter::convertTemperature(double value, const std::string& fromUnit, const std::string& toUnit) const {
    std::string from = sanitizeUnit(fromUnit);
    std::string to   = sanitizeUnit(toUnit);

    // Normalize source temperature to Kelvin
    double kelvin = 0.0;
    if (from == "c" || from == "celsius" || from == "centigrade" || from == "degc" || from == "°c") {
        kelvin = value + 273.15;
    } else if (from == "f" || from == "fahrenheit" || from == "degf" || from == "°f") {
        kelvin = (value - 32.0) * (5.0 / 9.0) + 273.15;
    } else if (from == "k" || from == "kelvin" || from == "degk") {
        kelvin = value;
    } else if (from == "r" || from == "rankine" || from == "degr" || from == "°r") {
        kelvin = value * (5.0 / 9.0);
    } else {
        throw ConversionError("Unknown source temperature unit: '" + fromUnit + "'");
    }

    // Convert Kelvin to target
    if (to == "c" || to == "celsius" || to == "centigrade" || to == "degc" || to == "°c") {
        return kelvin - 273.15;
    } else if (to == "f" || to == "fahrenheit" || to == "degf" || to == "°f") {
        return (kelvin - 273.15) * (9.0 / 5.0) + 32.0;
    } else if (to == "k" || to == "kelvin" || to == "degk") {
        return kelvin;
    } else if (to == "r" || to == "rankine" || to == "degr" || to == "°r") {
        return kelvin * (9.0 / 5.0);
    } else {
        throw ConversionError("Unknown target temperature unit: '" + toUnit + "'");
    }
}

double UnitConverter::convert(double value, const std::string& fromUnit, const std::string& toUnit) {
    std::string keyFrom = sanitizeUnit(fromUnit);
    std::string keyTo   = sanitizeUnit(toUnit);

    auto itFrom = unitLookup.find(keyFrom);
    if (itFrom == unitLookup.end()) {
        throw ConversionError("Unknown source unit '" + fromUnit + "'");
    }

    auto itTo = unitLookup.find(keyTo);
    if (itTo == unitLookup.end()) {
        throw ConversionError("Unknown target unit '" + toUnit + "'");
    }

    if (itFrom->second.first != itTo->second.first) {
        std::string catFrom = getCategoryName(itFrom->second.first);
        std::string catTo   = getCategoryName(itTo->second.first);
        throw ConversionError("Cannot convert between " + catFrom + " and " + catTo);
    }

    Category cat = itFrom->second.first;
    if (cat == Category::Temperature) {
        return convertTemperature(value, fromUnit, toUnit);
    }

    const auto& catInfo = getCategoryInfo(cat);
    double factorFrom = catInfo.units[itFrom->second.second].factorToBase;
    double factorTo   = catInfo.units[itTo->second.second].factorToBase;

    return (value * factorFrom) / factorTo;
}

double UnitConverter::convert(double value, const std::string& fromUnit, const std::string& toUnit, Category category) {
    if (category == Category::Temperature) {
        return convertTemperature(value, fromUnit, toUnit);
    }
    return convert(value, fromUnit, toUnit);
}

std::string UnitConverter::formatValue(double value, int precision) {
    if (std::isnan(value)) return "NaN";
    if (std::isinf(value)) return (value > 0) ? "Infinity" : "-Infinity";

    double absVal = std::abs(value);
    if (absVal != 0.0 && (absVal >= 1e12 || absVal < 1e-5)) {
        std::ostringstream ss;
        ss << std::scientific << std::setprecision(precision) << value;
        return ss.str();
    }

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string s = ss.str();

    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
    }
    if (s == "-0") s = "0";
    return s;
}

} // namespace Units
