#include "History.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace AppHistory {

HistoryManager::HistoryManager() = default;

std::string HistoryManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &in_time_t);
#else
    localtime_r(&in_time_t, &timeInfo);
#endif

    std::ostringstream ss;
    ss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void HistoryManager::addEntry(const std::string& type, const std::string& expression, const std::string& result) {
    entries.push_back({getCurrentTimestamp(), type, expression, result});
}

const std::vector<HistoryEntry>& HistoryManager::getEntries() const {
    return entries;
}

void HistoryManager::clear() {
    entries.clear();
}

size_t HistoryManager::size() const {
    return entries.size();
}

bool HistoryManager::isEmpty() const {
    return entries.empty();
}

bool HistoryManager::exportToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        return false;
    }

    out << "========================================================\n";
    out << "       CALCULATOR & UNIT CONVERTER HISTORY LOG          \n";
    out << "       Generated: " << getCurrentTimestamp() << "\n";
    out << "========================================================\n\n";

    out << std::left << std::setw(22) << "Timestamp"
        << std::setw(14) << "Type"
        << std::setw(32) << "Expression / Input"
        << "Result\n";
    out << std::string(80, '-') << "\n";

    for (const auto& item : entries) {
        out << std::left << std::setw(22) << item.timestamp
            << std::setw(14) << item.type
            << std::setw(32) << item.expression
            << item.result << "\n";
    }

    out << "\nTotal records: " << entries.size() << "\n";
    return true;
}

} // namespace AppHistory
