#include "../src/Task.hpp"
#include "../src/TaskManager.hpp"
#include "../src/Storage.hpp"
#include <iostream>
#include <cassert>
#include <cstdio>

using namespace Todo;

int g_passed = 0;
int g_failed = 0;

#define ASSERT_TRUE(expr, msg) \
    do { \
        if (!(expr)) { \
            std::cerr << "  \033[31m[FAILED]\033[0m " << msg << "\n"; \
            g_failed++; \
        } else { \
            g_passed++; \
        } \
    } while (0)

void testCrud() {
    TaskManager manager;
    Task t;
    t.title = "Write tests";
    t.priority = Priority::High;
    t.category = "Dev";

    int id = manager.add(t);
    ASSERT_TRUE(id == 1, "Task ID should be 1");
    ASSERT_TRUE(manager.size() == 1, "Manager size should be 1");

    Task* fetched = manager.get(id);
    ASSERT_TRUE(fetched != nullptr, "Task should be found");
    ASSERT_TRUE(fetched->title == "Write tests", "Title should match");
    ASSERT_TRUE(fetched->status == TaskStatus::Pending, "Initial status Pending");

    // Toggle status
    manager.toggleStatus(id);
    ASSERT_TRUE(manager.get(id)->status == TaskStatus::InProgress, "Toggled to InProgress");

    manager.toggleStatus(id);
    ASSERT_TRUE(manager.get(id)->status == TaskStatus::Completed, "Toggled to Completed");

    // Update
    Task tUp = *fetched;
    tUp.title = "Write unit tests";
    ASSERT_TRUE(manager.update(tUp), "Update succeeds");
    ASSERT_TRUE(manager.get(id)->title == "Write unit tests", "Updated title matches");

    // Delete
    ASSERT_TRUE(manager.remove(id), "Delete succeeds");
    ASSERT_TRUE(manager.empty(), "Manager is empty");
}

void testSubtasksAndNotes() {
    TaskManager manager;
    Task t;
    t.title = "Project";
    int id = manager.add(t);

    ASSERT_TRUE(manager.addSubTask(id, "Subtask 1"), "Add subtask 1");
    ASSERT_TRUE(manager.addSubTask(id, "Subtask 2"), "Add subtask 2");
    ASSERT_TRUE(manager.addNote(id, "Initial note"), "Add note");

    Task* task = manager.get(id);
    ASSERT_TRUE(task->subtasks.size() == 2, "2 subtasks present");
    ASSERT_TRUE(task->notes.size() == 1 && task->notes[0] == "Initial note", "1 note present");
    ASSERT_TRUE(task->completedSubtasks() == 0, "0 completed");
    ASSERT_TRUE(task->subtaskProgressPercent() == 0, "0% progress");

    ASSERT_TRUE(manager.toggleSubTask(id, 1), "Toggle subtask 1");
    ASSERT_TRUE(manager.get(id)->completedSubtasks() == 1, "1 completed");
    ASSERT_TRUE(manager.get(id)->subtaskProgressPercent() == 50, "50% progress");

    ASSERT_TRUE(manager.removeSubTask(id, 2), "Delete subtask 2");
    ASSERT_TRUE(manager.get(id)->subtasks.size() == 1, "1 subtask remaining");
    ASSERT_TRUE(manager.get(id)->subtaskProgressPercent() == 100, "100% progress");
}

void testBulkOperations() {
    TaskManager manager;
    manager.add({ 0, "Task 1", "", Priority::Low, TaskStatus::Completed, "General", "", {}, {} });
    manager.add({ 0, "Task 2", "", Priority::High, TaskStatus::Pending, "General", "", {}, {} });
    manager.add({ 0, "Task 3", "", Priority::Urgent, TaskStatus::Completed, "General", "", {}, {} });

    ASSERT_TRUE(manager.size() == 3, "3 tasks initially");

    // Clear completed
    int cleared = manager.clearCompleted();
    ASSERT_TRUE(cleared == 2, "Cleared 2 completed tasks");
    ASSERT_TRUE(manager.size() == 1, "1 task left");

    // Mark all completed
    manager.add({ 0, "Task 4", "", Priority::Medium, TaskStatus::Pending, "General", "", {}, {} });
    int marked = manager.markAllCompleted();
    ASSERT_TRUE(marked == 2, "Marked 2 tasks completed");
    ASSERT_TRUE(manager.get(2)->status == TaskStatus::Completed, "Task 2 is completed");
    ASSERT_TRUE(manager.get(4)->status == TaskStatus::Completed, "Task 4 is completed");

    // Bulk delete
    int deleted = manager.bulkDelete({ 2, 4 });
    ASSERT_TRUE(deleted == 2, "Bulk deleted 2 tasks");
    ASSERT_TRUE(manager.empty(), "Manager is empty");
}

void testQueriesAndSort() {
    TaskManager manager;
    manager.add({ 0, "Alpha", "A desc", Priority::Low, TaskStatus::Pending, "Work", "2026-09-10", {}, {} });
    manager.add({ 0, "Beta urgent", "B desc", Priority::Urgent, TaskStatus::InProgress, "Dev", "2026-09-02", {}, {} });
    manager.add({ 0, "Gamma", "G desc", Priority::Medium, TaskStatus::Completed, "Work", "2026-09-05", {}, {} });

    // Category filter
    auto workTasks = manager.query("Work");
    ASSERT_TRUE(workTasks.size() == 2, "2 Work tasks");

    // Search query
    auto searchTasks = manager.query("", std::nullopt, std::nullopt, "urgent");
    ASSERT_TRUE(searchTasks.size() == 1 && searchTasks[0].title == "Beta urgent", "Search query matches");

    // Sort by Due Date
    auto sortedDate = manager.query("", std::nullopt, std::nullopt, "", true);
    ASSERT_TRUE(sortedDate[0].title == "Beta urgent", "Earliest date first");
    ASSERT_TRUE(sortedDate[2].title == "Alpha", "Latest date last");
}

void testStorageAndExport() {
    std::string testFile = "test_tasks.dat";
    std::string testMd = "test_export.md";
    std::vector<Task> tasks;
    tasks.push_back({ 1, "File test", "Description with \n newline and | pipe", Priority::Urgent, TaskStatus::InProgress, "Dev", "2026-09-01", { { 1, "Sub 1", true } }, { "Note A" } });

    ASSERT_TRUE(Storage::save(testFile, tasks), "Save to file");

    std::vector<Task> loaded;
    ASSERT_TRUE(Storage::load(testFile, loaded), "Load from file");
    ASSERT_TRUE(loaded.size() == 1, "Loaded 1 task");
    ASSERT_TRUE(loaded[0].title == "File test", "Title matches");
    ASSERT_TRUE(loaded[0].notes.size() == 1 && loaded[0].notes[0] == "Note A", "Note preserved");

    ASSERT_TRUE(Storage::exportMarkdown(testMd, loaded), "Export markdown");

    std::remove(testFile.c_str());
    std::remove(testMd.c_str());
}

int main() {
    std::cout << "\n\033[36m=== Running Lean To-Do Tests (Ponytail) ===\033[0m\n\n";

    testCrud();
    testSubtasksAndNotes();
    testBulkOperations();
    testQueriesAndSort();
    testStorageAndExport();

    std::cout << "\n\033[32mPassed: " << g_passed << "\033[0m | \033[31mFailed: " << g_failed << "\033[0m\n\n";
    return (g_failed == 0) ? 0 : 1;
}
