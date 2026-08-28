#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Todo {

    enum class Priority { Low = 1, Medium = 2, High = 3, Urgent = 4 };
    enum class TaskStatus { Pending = 0, InProgress = 1, Completed = 2 };

    inline std::string priorityToString(Priority p) {
        switch (p) {
            case Priority::Low:    return "Low";
            case Priority::Medium: return "Medium";
            case Priority::High:   return "High";
            case Priority::Urgent: return "Urgent";
        }
        return "Medium";
    }

    inline Priority stringToPriority(const std::string& s) {
        if (s == "1" || s == "Low" || s == "low" || s == "l") return Priority::Low;
        if (s == "3" || s == "High" || s == "high" || s == "h") return Priority::High;
        if (s == "4" || s == "Urgent" || s == "urgent" || s == "u") return Priority::Urgent;
        return Priority::Medium;
    }

    inline std::string statusToString(TaskStatus s) {
        switch (s) {
            case TaskStatus::Pending:    return "Pending";
            case TaskStatus::InProgress: return "In Progress";
            case TaskStatus::Completed:  return "Completed";
        }
        return "Pending";
    }

    inline TaskStatus stringToStatus(const std::string& s) {
        if (s == "1" || s == "InProgress" || s == "doing") return TaskStatus::InProgress;
        if (s == "2" || s == "Completed" || s == "done") return TaskStatus::Completed;
        return TaskStatus::Pending;
    }

    inline std::string getTodayDate() {
        std::time_t t = std::time(nullptr);
        std::tm tmNow{};
#ifdef _WIN32
        localtime_s(&tmNow, &t);
#else
        localtime_r(&t, &tmNow);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tmNow, "%Y-%m-%d");
        return ss.str();
    }

    struct SubTask {
        int id{0};
        std::string title;
        bool isDone{false};
    };

    struct Task {
        int id{0};
        std::string title;
        std::string description;
        Priority priority{Priority::Medium};
        TaskStatus status{TaskStatus::Pending};
        std::string category{"General"};
        std::string dueDate; // YYYY-MM-DD
        std::vector<SubTask> subtasks;
        std::vector<std::string> notes;

        int completedSubtasks() const {
            int count = 0;
            for (const auto& st : subtasks) if (st.isDone) count++;
            return count;
        }

        int subtaskProgressPercent() const {
            if (subtasks.empty()) return (status == TaskStatus::Completed) ? 100 : 0;
            return (completedSubtasks() * 100) / static_cast<int>(subtasks.size());
        }

        bool isOverdue() const {
            if (status == TaskStatus::Completed || dueDate.empty()) return false;
            return dueDate < getTodayDate();
        }
    };

} // namespace Todo
