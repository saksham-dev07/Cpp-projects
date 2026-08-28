#include "TaskManager.hpp"
#include "Storage.hpp"
#include <algorithm>
#include <set>
#include <cctype>

namespace Todo {

    static std::string toLower(const std::string& str) {
        std::string s = str;
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    TaskManager::TaskManager() {
        updateNextId();
    }

    void TaskManager::updateNextId() {
        int maxId = 0;
        for (const auto& t : m_tasks) if (t.id > maxId) maxId = t.id;
        m_nextId = maxId + 1;
    }

    int TaskManager::add(Task task) {
        if (task.id <= 0) task.id = m_nextId++;
        else if (task.id >= m_nextId) m_nextId = task.id + 1;

        if (task.category.empty()) task.category = "General";

        int subId = 1;
        for (auto& st : task.subtasks) {
            if (st.id <= 0) st.id = subId++;
            else if (st.id >= subId) subId = st.id + 1;
        }

        m_tasks.push_back(task);
        save();
        return task.id;
    }

    bool TaskManager::update(const Task& task) {
        for (auto& t : m_tasks) {
            if (t.id == task.id) {
                t = task;
                save();
                return true;
            }
        }
        return false;
    }

    bool TaskManager::remove(int id) {
        auto it = std::find_if(m_tasks.begin(), m_tasks.end(), [id](const Task& t) { return t.id == id; });
        if (it != m_tasks.end()) {
            m_tasks.erase(it);
            save();
            return true;
        }
        return false;
    }

    bool TaskManager::toggleStatus(int id) {
        for (auto& t : m_tasks) {
            if (t.id == id) {
                if (t.status == TaskStatus::Pending) t.status = TaskStatus::InProgress;
                else if (t.status == TaskStatus::InProgress) {
                    t.status = TaskStatus::Completed;
                    for (auto& st : t.subtasks) st.isDone = true;
                } else t.status = TaskStatus::Pending;
                save();
                return true;
            }
        }
        return false;
    }

    bool TaskManager::addSubTask(int taskId, const std::string& title) {
        if (title.empty()) return false;
        for (auto& t : m_tasks) {
            if (t.id == taskId) {
                int nextSubId = 1;
                for (const auto& st : t.subtasks) {
                    if (st.id >= nextSubId) nextSubId = st.id + 1;
                }
                t.subtasks.push_back({ nextSubId, title, false });
                save();
                return true;
            }
        }
        return false;
    }

    bool TaskManager::toggleSubTask(int taskId, int subId) {
        for (auto& t : m_tasks) {
            if (t.id == taskId) {
                for (auto& st : t.subtasks) {
                    if (st.id == subId) {
                        st.isDone = !st.isDone;
                        save();
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TaskManager::removeSubTask(int taskId, int subId) {
        for (auto& t : m_tasks) {
            if (t.id == taskId) {
                auto it = std::find_if(t.subtasks.begin(), t.subtasks.end(), [subId](const SubTask& s) { return s.id == subId; });
                if (it != t.subtasks.end()) {
                    t.subtasks.erase(it);
                    save();
                    return true;
                }
            }
        }
        return false;
    }

    bool TaskManager::addNote(int taskId, const std::string& note) {
        if (note.empty()) return false;
        for (auto& t : m_tasks) {
            if (t.id == taskId) {
                t.notes.push_back(note);
                save();
                return true;
            }
        }
        return false;
    }

    int TaskManager::clearCompleted() {
        size_t initial = m_tasks.size();
        m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(), [](const Task& t) { return t.status == TaskStatus::Completed; }), m_tasks.end());
        int removed = static_cast<int>(initial - m_tasks.size());
        if (removed > 0) save();
        return removed;
    }

    int TaskManager::markAllCompleted() {
        int count = 0;
        for (auto& t : m_tasks) {
            if (t.status != TaskStatus::Completed) {
                t.status = TaskStatus::Completed;
                for (auto& st : t.subtasks) st.isDone = true;
                count++;
            }
        }
        if (count > 0) save();
        return count;
    }

    int TaskManager::bulkDelete(const std::vector<int>& ids) {
        int count = 0;
        for (int id : ids) {
            auto it = std::find_if(m_tasks.begin(), m_tasks.end(), [id](const Task& t) { return t.id == id; });
            if (it != m_tasks.end()) {
                m_tasks.erase(it);
                count++;
            }
        }
        if (count > 0) save();
        return count;
    }

    Task* TaskManager::get(int id) {
        for (auto& t : m_tasks) if (t.id == id) return &t;
        return nullptr;
    }

    const Task* TaskManager::get(int id) const {
        for (const auto& t : m_tasks) if (t.id == id) return &t;
        return nullptr;
    }

    std::vector<Task> TaskManager::query(
        const std::string& category,
        std::optional<Priority> priority,
        std::optional<TaskStatus> status,
        const std::string& search,
        bool sortByDate
    ) const {
        std::vector<Task> res;
        std::string lowCat = toLower(category);
        std::string lowSearch = toLower(search);

        for (const auto& t : m_tasks) {
            if (!category.empty() && toLower(t.category) != lowCat) continue;
            if (priority.has_value() && t.priority != *priority) continue;
            if (status.has_value() && t.status != *status) continue;
            if (!search.empty()) {
                if (toLower(t.title).find(lowSearch) == std::string::npos &&
                    toLower(t.description).find(lowSearch) == std::string::npos &&
                    toLower(t.category).find(lowSearch) == std::string::npos) {
                    continue;
                }
            }
            res.push_back(t);
        }

        if (sortByDate) {
            std::sort(res.begin(), res.end(), [](const Task& a, const Task& b) {
                if (a.dueDate.empty() && !b.dueDate.empty()) return false;
                if (!a.dueDate.empty() && b.dueDate.empty()) return true;
                return a.dueDate < b.dueDate;
            });
        } else {
            // Default sort: highest priority first, then ID
            std::sort(res.begin(), res.end(), [](const Task& a, const Task& b) {
                if (a.priority != b.priority) {
                    return static_cast<int>(a.priority) > static_cast<int>(b.priority);
                }
                return a.id < b.id;
            });
        }

        return res;
    }

    std::vector<std::string> TaskManager::categories() const {
        std::set<std::string> cats;
        for (const auto& t : m_tasks) if (!t.category.empty()) cats.insert(t.category);
        return std::vector<std::string>(cats.begin(), cats.end());
    }

    void TaskManager::getStats(int& total, int& pending, int& inProgress, int& completed, int& overdue) const {
        total = static_cast<int>(m_tasks.size());
        pending = 0; inProgress = 0; completed = 0; overdue = 0;
        for (const auto& t : m_tasks) {
            if (t.status == TaskStatus::Pending) pending++;
            else if (t.status == TaskStatus::InProgress) inProgress++;
            else if (t.status == TaskStatus::Completed) completed++;

            if (t.isOverdue()) overdue++;
        }
    }

    bool TaskManager::load(const std::string& filename) {
        m_filename = filename;
        bool ok = Storage::load(m_filename, m_tasks);
        updateNextId();
        return ok;
    }

    bool TaskManager::save(const std::string& filename) {
        m_filename = filename;
        return Storage::save(m_filename, m_tasks);
    }

} // namespace Todo
