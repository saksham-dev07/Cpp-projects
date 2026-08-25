#pragma once

#include <string>
#include <vector>

namespace AppHistory {

struct HistoryEntry {
    std::string timestamp;
    std::string type;       // "Math", "Unit", "Programmer", "Finance", "Health"
    std::string expression; // e.g. "2 * (3 + 4)" or "100 km -> mi"
    std::string result;     // e.g. "14" or "62.1371"
};

class HistoryManager {
public:
    HistoryManager();

    void addEntry(const std::string& type, const std::string& expression, const std::string& result);
    const std::vector<HistoryEntry>& getEntries() const;
    void clear();
    size_t size() const;
    bool isEmpty() const;

    bool exportToFile(const std::string& filename) const;
    static std::string getCurrentTimestamp();

private:
    std::vector<HistoryEntry> entries;
};

} // namespace AppHistory
