#include "Storage.hpp"
#include <fstream>
#include <sstream>

namespace Todo {

    static std::string escape(const std::string& str) {
        std::string res;
        for (char c : str) {
            if (c == '\n') res += "\\n";
            else if (c == '|') res += "\\p";
            else if (c == '\\') res += "\\\\";
            else res += c;
        }
        return res;
    }

    static std::string unescape(const std::string& str) {
        std::string res;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                char next = str[++i];
                if (next == 'n') res += '\n';
                else if (next == 'p') res += '|';
                else res += next;
            } else {
                res += str[i];
            }
        }
        return res;
    }

    bool Storage::save(const std::string& filename, const std::vector<Task>& tasks) {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) return false;

        for (const auto& t : tasks) {
            ofs << "TASK|" << t.id << "|"
                << escape(t.title) << "|"
                << escape(t.description) << "|"
                << static_cast<int>(t.priority) << "|"
                << static_cast<int>(t.status) << "|"
                << escape(t.category) << "|"
                << escape(t.dueDate) << "\n";

            for (const auto& st : t.subtasks) {
                ofs << "SUB|" << st.id << "|"
                    << escape(st.title) << "|"
                    << (st.isDone ? "1" : "0") << "\n";
            }

            for (const auto& note : t.notes) {
                ofs << "NOTE|" << escape(note) << "\n";
            }
        }
        return true;
    }

    bool Storage::load(const std::string& filename, std::vector<Task>& tasks) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) return false;

        tasks.clear();
        std::string line;

        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string type;
            if (!std::getline(ss, type, '|')) continue;

            if (type == "TASK") {
                Task t;
                std::string sId, sTitle, sDesc, sPri, sStat, sCat, sDue;
                std::getline(ss, sId, '|');
                std::getline(ss, sTitle, '|');
                std::getline(ss, sDesc, '|');
                std::getline(ss, sPri, '|');
                std::getline(ss, sStat, '|');
                std::getline(ss, sCat, '|');
                std::getline(ss, sDue, '|');

                try {
                    t.id = std::stoi(sId);
                    t.title = unescape(sTitle);
                    t.description = unescape(sDesc);
                    t.priority = static_cast<Priority>(std::stoi(sPri));
                    t.status = static_cast<TaskStatus>(std::stoi(sStat));
                    t.category = unescape(sCat);
                    t.dueDate = unescape(sDue);
                    tasks.push_back(t);
                } catch (...) {}
            } else if (type == "SUB" && !tasks.empty()) {
                SubTask st;
                std::string sId, sTitle, sDone;
                std::getline(ss, sId, '|');
                std::getline(ss, sTitle, '|');
                std::getline(ss, sDone, '|');

                try {
                    st.id = std::stoi(sId);
                    st.title = unescape(sTitle);
                    st.isDone = (sDone == "1");
                    tasks.back().subtasks.push_back(st);
                } catch (...) {}
            } else if (type == "NOTE" && !tasks.empty()) {
                std::string note;
                std::getline(ss, note);
                tasks.back().notes.push_back(unescape(note));
            }
        }
        return true;
    }

    bool Storage::exportMarkdown(const std::string& filename, const std::vector<Task>& tasks) {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) return false;

        ofs << "# 📋 To-Do List Export\n\n";
        for (const auto& t : tasks) {
            std::string check = (t.status == TaskStatus::Completed) ? "[x]" : "[ ]";
            ofs << "### " << check << " #" << t.id << " " << t.title << "\n";
            ofs << "- **Status**: `" << statusToString(t.status) << "` | **Priority**: `" << priorityToString(t.priority) << "` | **Category**: `@" << t.category << "`\n";
            if (!t.dueDate.empty()) {
                ofs << "- **Due Date**: " << (t.isOverdue() ? "⚠️ **OVERDUE** " : "") << t.dueDate << "\n";
            }
            if (!t.description.empty()) ofs << "> " << t.description << "\n";
            if (!t.subtasks.empty()) {
                ofs << "\n**Checklist:**\n";
                for (const auto& st : t.subtasks) {
                    ofs << "- " << (st.isDone ? "[x] " : "[ ] ") << st.title << "\n";
                }
            }
            if (!t.notes.empty()) {
                ofs << "\n**Notes:**\n";
                for (const auto& n : t.notes) ofs << "- " << n << "\n";
            }
            ofs << "\n---\n\n";
        }
        return true;
    }

} // namespace Todo
