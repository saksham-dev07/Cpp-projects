#include "UI.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace Todo {

    namespace Color {
        const char* Reset       = "\033[0m";
        const char* Bold        = "\033[1m";
        const char* Dim         = "\033[2m";
        const char* Cyan        = "\033[38;2;0;220;255m";
        const char* Green       = "\033[38;2;46;204;113m";
        const char* Yellow      = "\033[38;2;241;196;15m";
        const char* Orange      = "\033[38;2;230;126;34m";
        const char* Red         = "\033[38;2;231;76;60m";
        const char* UrgentBg    = "\033[48;2;140;20;20m\033[38;2;255;255;255m\033[1m";
        const char* Magenta     = "\033[38;2;189;147;249m";
        const char* Gray        = "\033[38;2;120;130;140m";
        const char* White       = "\033[38;2;240;240;240m";
        const char* Border      = "\033[38;2;80;100;120m";
    }

    void UI::init() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode)) {
                SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
            }
        }
#endif
    }

    void UI::clear() {
        std::cout << "\033[2J\033[H" << std::flush;
    }

    void UI::printBanner() {
        std::cout << Color::Cyan << R"(
  ┌──────────────────────────────────────────────────────────┐
  │         📋  TASKFLOW - TO-DO & PRODUCTIVITY CLI          │
  └──────────────────────────────────────────────────────────┘
)" << Color::Reset;
    }

    void UI::printHeader(const std::string& title, const std::string& subtitle) {
        int width = 64;
        std::string bar(width - 4, '-');
        std::cout << "\n  " << Color::Border << "+" << bar << "+\n";
        
        // Centered title
        int padL = (width - 4 - static_cast<int>(title.length())) / 2;
        if (padL < 0) padL = 0;
        int padR = width - 4 - static_cast<int>(title.length()) - padL;
        if (padR < 0) padR = 0;
        
        std::cout << "  | " << std::string(padL, ' ') << Color::Cyan << Color::Bold << title 
                  << Color::Reset << std::string(padR, ' ') << Color::Border << " |\n";

        if (!subtitle.empty()) {
            int sPadL = (width - 4 - static_cast<int>(subtitle.length())) / 2;
            if (sPadL < 0) sPadL = 0;
            int sPadR = width - 4 - static_cast<int>(subtitle.length()) - sPadL;
            if (sPadR < 0) sPadR = 0;
            std::cout << "  | " << std::string(sPadL, ' ') << Color::Gray << subtitle 
                      << std::string(sPadR, ' ') << Color::Border << " |\n";
        }
        
        std::cout << "  +" << bar << "+\n" << Color::Reset << "\n";
    }

    static std::string formatPriorityBadge(Priority p) {
        switch (p) {
            case Priority::Low:    return std::string(Color::Green) + "[LOW]" + Color::Reset;
            case Priority::Medium: return std::string(Color::Yellow) + "[MED]" + Color::Reset;
            case Priority::High:   return std::string(Color::Orange) + "[HIGH]" + Color::Reset;
            case Priority::Urgent: return std::string(Color::UrgentBg) + "[URGENT]" + Color::Reset;
        }
        return "[MED]";
    }

    static std::string formatStatusBadge(TaskStatus s) {
        switch (s) {
            case TaskStatus::Pending:    return std::string(Color::Gray) + "[ ] Pending" + Color::Reset;
            case TaskStatus::InProgress: return std::string(Color::Yellow) + "[-] Doing" + Color::Reset;
            case TaskStatus::Completed:  return std::string(Color::Green) + "[v] Done" + Color::Reset;
        }
        return "[ ] Pending";
    }

    static std::string formatChecklistBar(int done, int total) {
        if (total == 0) return std::string(Color::Gray) + "-" + Color::Reset;
        int width = 5;
        int filled = (done * width) / total;
        int empty = width - filled;
        int pct = (done * 100) / total;

        std::string bar = std::string(Color::Gray) + "[";
        bar += (done == total) ? Color::Green : Color::Yellow;
        for (int i = 0; i < filled; ++i) bar += "=";
        bar += Color::Gray;
        for (int i = 0; i < empty; ++i) bar += ".";
        bar += "] " + std::to_string(done) + "/" + std::to_string(total) + " (" + std::to_string(pct) + "%)" + Color::Reset;
        return bar;
    }

    void UI::printTasks(const std::vector<Task>& tasks) {
        if (tasks.empty()) {
            std::cout << Color::Gray << "    (No tasks found matching current criteria)\n\n" << Color::Reset;
            return;
        }

        std::cout << "  " << Color::Border << "+------+-------------+----------+----------------------------+--------------+------------------+-----------------+\n";
        std::cout << "  | " << Color::Bold << Color::Cyan << "ID   " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Status      " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Priority " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Title                       " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Category     " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Due Date         " << Color::Border << "| "
                  << Color::Bold << Color::Cyan << "Checklist       " << Color::Border << "|\n";
        std::cout << "  +------+-------------+----------+----------------------------+--------------+------------------+-----------------+\n" << Color::Reset;

        for (const auto& t : tasks) {
            std::string titleTrunc = t.title.length() > 26 ? t.title.substr(0, 23) + "..." : t.title;
            std::string due = t.dueDate.empty() ? "-" : (t.isOverdue() ? (t.dueDate + " [!]") : t.dueDate);
            std::string cat = "@" + t.category;
            if (cat.length() > 12) cat = cat.substr(0, 9) + "...";

            // Format padded cells
            std::ostringstream ssId; ssId << std::left << std::setw(5) << t.id;
            std::ostringstream ssTitle; ssTitle << std::left << std::setw(27) << titleTrunc;
            std::ostringstream ssCat; ssCat << std::left << std::setw(13) << cat;
            std::ostringstream ssDue; ssDue << std::left << std::setw(17) << due;

            std::string statusBadge = (t.status == TaskStatus::Completed) ? (std::string(Color::Green) + "[v] Done   ") :
                                      (t.status == TaskStatus::InProgress) ? (std::string(Color::Yellow) + "[-] Doing  ") :
                                      (std::string(Color::Gray) + "[ ] Pending");

            std::string priBadge = (t.priority == Priority::Urgent) ? (std::string(Color::UrgentBg) + "[URGENT] " + Color::Reset) :
                                   (t.priority == Priority::High) ? (std::string(Color::Orange) + "[HIGH]   " + Color::Reset) :
                                   (t.priority == Priority::Medium) ? (std::string(Color::Yellow) + "[MED]    " + Color::Reset) :
                                   (std::string(Color::Green) + "[LOW]    " + Color::Reset);

            std::cout << "  " << Color::Border << "| " << Color::White << ssId.str() << Color::Border << "| "
                      << statusBadge << Color::Reset << Color::Border << " | "
                      << priBadge << Color::Border << "| "
                      << (t.status == TaskStatus::Completed ? (std::string(Color::Gray) + "\033[9m" + ssTitle.str() + "\033[0m") : (std::string(Color::White) + ssTitle.str()))
                      << Color::Border << "| " << Color::Cyan << ssCat.str() << Color::Border << "| "
                      << (t.isOverdue() ? (std::string(Color::Red) + ssDue.str() + Color::Reset) : (std::string(Color::Gray) + ssDue.str() + Color::Reset))
                      << Color::Border << "| "
                      << formatChecklistBar(t.completedSubtasks(), static_cast<int>(t.subtasks.size()))
                      << " " << Color::Border << "|\n";
        }
        std::cout << "  +------+-------------+----------+----------------------------+--------------+------------------+-----------------+\n\n" << Color::Reset;
    }

    void UI::printTaskDetails(const Task& t) {
        std::cout << "\n  " << Color::Border << "+--------------------------------------------------------------+\n";
        std::cout << "  | " << Color::Cyan << Color::Bold << "TASK DETAILS #" << t.id << Color::Reset << "\n";
        std::cout << "  " << Color::Border << "+--------------------------------------------------------------+\n" << Color::Reset;
        std::cout << "    Title:       " << Color::White << Color::Bold << t.title << Color::Reset << "\n";
        std::cout << "    Status:      " << formatStatusBadge(t.status) << "\n";
        std::cout << "    Priority:    " << formatPriorityBadge(t.priority) << "\n";
        std::cout << "    Category:    " << Color::Cyan << "@" << t.category << Color::Reset << "\n";
        std::cout << "    Due Date:    " << (t.dueDate.empty() ? "None" : (t.dueDate + (t.isOverdue() ? (std::string(Color::Red) + " [OVERDUE]" + Color::Reset) : ""))) << "\n";
        if (!t.description.empty()) {
            std::cout << "    Description: " << Color::White << t.description << Color::Reset << "\n";
        }

        if (!t.subtasks.empty()) {
            std::cout << "\n    " << Color::Bold << "Checklist (" << t.completedSubtasks() << "/" << t.subtasks.size() << "):\n" << Color::Reset;
            for (const auto& st : t.subtasks) {
                std::cout << "      " << (st.isDone ? (std::string(Color::Green) + "[v] ") : (std::string(Color::Gray) + "[ ] "))
                          << "#" << st.id << " " << (st.isDone ? (std::string(Color::Gray) + "\033[9m" + st.title + "\033[0m") : (std::string(Color::White) + st.title))
                          << Color::Reset << "\n";
            }
        }

        if (!t.notes.empty()) {
            std::cout << "\n    " << Color::Bold << "Notes & Activity:\n" << Color::Reset;
            for (const auto& n : t.notes) {
                std::cout << "      " << Color::Cyan << "• " << Color::White << n << Color::Reset << "\n";
            }
        }
        std::cout << "  " << Color::Border << "+--------------------------------------------------------------+\n\n" << Color::Reset;
    }

    void UI::printStats(int total, int pending, int inProgress, int completed, int overdue) {
        double rate = (total > 0) ? (static_cast<double>(completed) * 100.0 / total) : 0.0;
        
        std::cout << "  " << Color::Border << "[ "
                  << Color::Cyan << "Total: " << Color::Bold << total << Color::Reset
                  << Color::Border << " | " << Color::Green << "Done: " << completed << " (" << std::fixed << std::setprecision(0) << rate << "%)" << Color::Reset
                  << Color::Border << " | " << Color::Yellow << "Doing: " << inProgress << Color::Reset
                  << Color::Border << " | " << Color::Gray << "Pending: " << pending << Color::Reset
                  << (overdue > 0 ? (std::string(Color::Border) + " | " + Color::Red + Color::Bold + "Overdue: " + std::to_string(overdue) + " [!]" + Color::Reset) : "")
                  << Color::Border << " ]\n\n" << Color::Reset;
    }

    int UI::getInt(const std::string& prompt, int min, int max, std::optional<int> defaultVal) {
        while (true) {
            std::cout << Color::White << "  " << prompt;
            if (defaultVal.has_value()) std::cout << Color::Gray << " [" << *defaultVal << "]";
            std::cout << Color::Cyan << " > " << Color::Reset << std::flush;

            std::string line;
            if (!std::getline(std::cin, line)) return defaultVal.value_or(min);
            if (line.empty() && defaultVal.has_value()) return *defaultVal;

            try {
                size_t pos = 0;
                int v = std::stoi(line, &pos);
                if (pos == line.length() && v >= min && v <= max) return v;
            } catch (...) {}
            std::cout << Color::Red << "    Invalid input. Enter a number between " << min << " and " << max << ".\n" << Color::Reset;
        }
    }

    std::string UI::getString(const std::string& prompt, bool allowEmpty, const std::string& defaultVal) {
        while (true) {
            std::cout << Color::White << "  " << prompt;
            if (!defaultVal.empty()) std::cout << Color::Gray << " [" << defaultVal << "]";
            std::cout << Color::Cyan << " > " << Color::Reset << std::flush;

            std::string line;
            if (!std::getline(std::cin, line)) return defaultVal;
            if (line.empty()) {
                if (!defaultVal.empty()) return defaultVal;
                if (allowEmpty) return "";
                std::cout << Color::Red << "    Field cannot be blank.\n" << Color::Reset;
                continue;
            }
            return line;
        }
    }

    std::string UI::getDate(const std::string& prompt, bool allowEmpty, const std::string& defaultVal) {
        std::string raw = getString(prompt + " (YYYY-MM-DD, 'today', 'tomorrow', '+3d', '+1w')", allowEmpty, defaultVal);
        if (raw.empty()) return defaultVal;

        if (raw == "today" || raw == "t") return getTodayDate();

        auto addDaysToDate = [](int days) {
            auto now = std::chrono::system_clock::now() + std::chrono::hours(24 * days);
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tmVal{};
#ifdef _WIN32
            localtime_s(&tmVal, &t);
#else
            localtime_r(&t, &tmVal);
#endif
            std::ostringstream ss;
            ss << std::put_time(&tmVal, "%Y-%m-%d");
            return ss.str();
        };

        if (raw == "tomorrow" || raw == "tom") return addDaysToDate(1);
        if (raw == "+1d") return addDaysToDate(1);
        if (raw == "+2d") return addDaysToDate(2);
        if (raw == "+3d") return addDaysToDate(3);
        if (raw == "+5d") return addDaysToDate(5);
        if (raw == "+1w") return addDaysToDate(7);
        if (raw == "+2w") return addDaysToDate(14);
        if (raw == "+1m") return addDaysToDate(30);

        return raw;
    }

    Priority UI::getPriority(const std::string& prompt, Priority def) {
        int val = getInt(prompt, 1, 4, static_cast<int>(def));
        return static_cast<Priority>(val);
    }

    TaskStatus UI::getStatus(const std::string& prompt, TaskStatus def) {
        int val = getInt(prompt, 0, 2, static_cast<int>(def));
        return static_cast<TaskStatus>(val);
    }

    bool UI::confirm(const std::string& prompt) {
        std::string ans = getString(prompt + " (y/N)", true, "n");
        return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
    }

} // namespace Todo
