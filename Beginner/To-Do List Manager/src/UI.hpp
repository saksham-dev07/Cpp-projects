#pragma once

#include "Task.hpp"
#include <vector>
#include <string>
#include <optional>

namespace Todo {

    class UI {
    public:
        static void init();
        static void clear();
        static void printBanner();
        static void printHeader(const std::string& title, const std::string& subtitle = "");
        static void printTasks(const std::vector<Task>& tasks);
        static void printTaskDetails(const Task& task);
        static void printStats(int total, int pending, int inProgress, int completed, int overdue);

        static int getInt(const std::string& prompt, int min, int max, std::optional<int> defaultVal = std::nullopt);
        static std::string getString(const std::string& prompt, bool allowEmpty = false, const std::string& defaultVal = "");
        static std::string getDate(const std::string& prompt, bool allowEmpty = true, const std::string& defaultVal = "");
        static Priority getPriority(const std::string& prompt = "Priority (1=Low, 2=Med, 3=High, 4=Urgent)", Priority def = Priority::Medium);
        static TaskStatus getStatus(const std::string& prompt = "Status (0=Pending, 1=InProgress, 2=Completed)", TaskStatus def = TaskStatus::Pending);
        static bool confirm(const std::string& prompt);
    };

} // namespace Todo
