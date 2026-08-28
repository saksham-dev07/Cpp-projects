#pragma once

#include "Task.hpp"
#include <vector>
#include <string>
#include <optional>

namespace Todo {

    class TaskManager {
    public:
        TaskManager();

        int add(Task task);
        bool update(const Task& task);
        bool remove(int id);
        bool toggleStatus(int id);

        bool addSubTask(int taskId, const std::string& title);
        bool toggleSubTask(int taskId, int subId);
        bool removeSubTask(int taskId, int subId);
        bool addNote(int taskId, const std::string& note);

        // Bulk operations
        int clearCompleted();
        int markAllCompleted();
        int bulkDelete(const std::vector<int>& ids);

        Task* get(int id);
        const Task* get(int id) const;
        const std::vector<Task>& all() const { return m_tasks; }
        size_t size() const { return m_tasks.size(); }
        bool empty() const { return m_tasks.empty(); }

        std::vector<Task> query(
            const std::string& category = "",
            std::optional<Priority> priority = std::nullopt,
            std::optional<TaskStatus> status = std::nullopt,
            const std::string& search = "",
            bool sortByDate = false
        ) const;

        std::vector<std::string> categories() const;

        void getStats(int& total, int& pending, int& inProgress, int& completed, int& overdue) const;

        bool load(const std::string& filename = "tasks.dat");
        bool save(const std::string& filename = "tasks.dat");

    private:
        std::vector<Task> m_tasks;
        int m_nextId{1};
        std::string m_filename{"tasks.dat"};
        void updateNextId();
    };

} // namespace Todo
