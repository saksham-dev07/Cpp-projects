#include "TaskManager.hpp"
#include "Storage.hpp"
#include "UI.hpp"
#include <iostream>
#include <sstream>

using namespace Todo;

void handleView(TaskManager& manager) {
    std::string catFilter;
    std::string searchFilter;
    bool sortByDate = false;

    while (true) {
        UI::clear();
        std::string sub = (catFilter.empty() ? "All Categories" : ("@" + catFilter));
        if (!searchFilter.empty()) sub += " | Matching: '" + searchFilter + "'";
        UI::printHeader("TASK EXPLORER", sub);

        auto tasks = manager.query(catFilter, std::nullopt, std::nullopt, searchFilter, sortByDate);
        UI::printTasks(tasks);

        std::cout << "  \033[1mCommands:\033[0m \033[36m[t]\033[0m Toggle Status | \033[36m[v]\033[0m View Details | \033[36m[n]\033[0m Add Note | \033[36m[f]\033[0m Filter | \033[36m[s]\033[0m Sort ("
                  << (sortByDate ? "\033[33mDue Date\033[0m" : "\033[35mPriority\033[0m") << ") | \033[31m[q]\033[0m Back\n";
        std::string act = UI::getString("Action", true, "q");

        if (act == "q" || act == "0") break;
        if (act == "s") {
            sortByDate = !sortByDate;
        } else if (act == "f") {
            catFilter = UI::getString("Filter by Category (empty for all)", true);
            searchFilter = UI::getString("Search Keyword (empty for all)", true);
        } else if (act == "t") {
            int id = UI::getInt("Task ID to toggle", 1, 999999);
            if (!manager.toggleStatus(id)) std::cout << "  Task not found.\n";
        } else if (act == "n") {
            int id = UI::getInt("Task ID", 1, 999999);
            std::string note = UI::getString("Note text");
            if (manager.addNote(id, note)) std::cout << "  Note added.\n";
            else std::cout << "  Task not found.\n";
            UI::getString("Press Enter...", true);
        } else if (act == "v") {
            int id = UI::getInt("Task ID to view", 1, 999999);
            const Task* t = manager.get(id);
            if (t) {
                UI::printTaskDetails(*t);
                UI::getString("Press Enter to return...", true);
            } else {
                std::cout << "  Task not found.\n";
                UI::getString("Press Enter...", true);
            }
        }
    }
}

void handleAdd(TaskManager& manager) {
    UI::clear();
    UI::printHeader("ADD NEW TASK");

    Task t;
    t.title = UI::getString("Title");
    t.description = UI::getString("Description (optional)", true);
    t.priority = UI::getPriority();
    t.category = UI::getString("Category", true, "General");
    t.dueDate = UI::getDate("Due Date", true);

    if (UI::confirm("Add subtasks now?")) {
        int idx = 1;
        while (true) {
            std::string st = UI::getString("Subtask #" + std::to_string(idx) + " (empty to finish)", true);
            if (st.empty()) break;
            t.subtasks.push_back({ idx++, st, false });
        }
    }

    int id = manager.add(t);
    std::cout << "\n  \033[32m[✓] Task #" << id << " added successfully!\033[0m\n";
    UI::getString("Press Enter to continue...", true);
}

void handleEdit(TaskManager& manager) {
    UI::clear();
    UI::printHeader("EDIT TASK");

    int id = UI::getInt("Enter Task ID to edit", 1, 999999);
    Task* t = manager.get(id);
    if (!t) {
        std::cout << "  Task not found.\n";
        UI::getString("Press Enter...", true);
        return;
    }

    t->title = UI::getString("New Title", false, t->title);
    t->description = UI::getString("New Description", true, t->description);
    t->priority = UI::getPriority("New Priority", t->priority);
    t->status = UI::getStatus("New Status", t->status);
    t->category = UI::getString("New Category", true, t->category);
    t->dueDate = UI::getDate("New Due Date", true, t->dueDate);

    manager.update(*t);
    std::cout << "\n  \033[32m[✓] Task #" << id << " updated.\033[0m\n";
    UI::getString("Press Enter to continue...", true);
}

void handleDelete(TaskManager& manager) {
    UI::clear();
    UI::printHeader("DELETE TASK");

    int id = UI::getInt("Enter Task ID to delete", 1, 999999);
    if (UI::confirm("Are you sure you want to delete Task #" + std::to_string(id) + "?")) {
        if (manager.remove(id)) {
            std::cout << "\n  \033[32m[✓] Task #" << id << " deleted.\033[0m\n";
        } else {
            std::cout << "\n  Task not found.\n";
        }
    }
    UI::getString("Press Enter to continue...", true);
}

void handleSubtasks(TaskManager& manager) {
    UI::clear();
    UI::printHeader("MANAGE SUBTASKS & NOTES");

    int id = UI::getInt("Enter Task ID", 1, 999999);
    Task* t = manager.get(id);
    if (!t) {
        std::cout << "  Task not found.\n";
        UI::getString("Press Enter...", true);
        return;
    }

    UI::printTaskDetails(*t);
    std::cout << "  \033[36m1)\033[0m Add Subtask | \033[36m2)\033[0m Toggle Subtask Done | \033[36m3)\033[0m Delete Subtask | \033[36m4)\033[0m Add Note | \033[31m0)\033[0m Back\n";
    int opt = UI::getInt("Choice", 0, 4, 0);

    if (opt == 1) {
        std::string title = UI::getString("Subtask title");
        manager.addSubTask(id, title);
    } else if (opt == 2) {
        int subId = UI::getInt("Subtask # to toggle", 1, 999);
        manager.toggleSubTask(id, subId);
    } else if (opt == 3) {
        int subId = UI::getInt("Subtask # to delete", 1, 999);
        manager.removeSubTask(id, subId);
    } else if (opt == 4) {
        std::string note = UI::getString("Note text");
        manager.addNote(id, note);
    }
}

void handleBulk(TaskManager& manager) {
    UI::clear();
    UI::printHeader("BULK OPERATIONS");

    std::cout << "  \033[36m1)\033[0m Clear all Completed Tasks\n";
    std::cout << "  \033[36m2)\033[0m Mark all Tasks as Completed\n";
    std::cout << "  \033[36m3)\033[0m Bulk Delete by IDs (e.g. 1, 3, 5)\n";
    std::cout << "  \033[31m0)\033[0m Back\n\n";

    int choice = UI::getInt("Option", 0, 3, 0);
    if (choice == 1) {
        int count = manager.clearCompleted();
        std::cout << "\n  \033[32m[✓] Cleared " << count << " completed tasks.\033[0m\n";
    } else if (choice == 2) {
        int count = manager.markAllCompleted();
        std::cout << "\n  \033[32m[✓] Marked " << count << " tasks as completed.\033[0m\n";
    } else if (choice == 3) {
        std::string idsStr = UI::getString("Enter comma-separated IDs");
        std::stringstream ss(idsStr);
        std::string item;
        std::vector<int> ids;
        while (std::getline(ss, item, ',')) {
            try { ids.push_back(std::stoi(item)); } catch (...) {}
        }
        int count = manager.bulkDelete(ids);
        std::cout << "\n  \033[32m[✓] Deleted " << count << " tasks.\033[0m\n";
    }
    UI::getString("Press Enter to continue...", true);
}

void handleExport(TaskManager& manager) {
    UI::clear();
    UI::printHeader("EXPORT TASKS");

    std::string filename = UI::getString("Output Markdown filename", false, "todo_export.md");
    if (Storage::exportMarkdown(filename, manager.all())) {
        std::cout << "\n  \033[32m[✓] Successfully exported " << manager.size() << " tasks to " << filename << "!\033[0m\n";
    } else {
        std::cout << "\n  \033[31m[!] Failed to export to " << filename << ".\033[0m\n";
    }
    UI::getString("Press Enter to continue...", true);
}

int main() {
    UI::init();
    TaskManager manager;
    manager.load("tasks.dat");

    if (manager.empty()) {
        manager.add({ 0, "Build lean C++ To-Do Manager", "Apply Ponytail guidelines", Priority::Urgent, TaskStatus::InProgress, "Dev", "2026-09-01", { { 1, "Design structs", true }, { 2, "Test & verify", false } }, { "Lean and standard library only" } });
        manager.add({ 0, "Code Review & Refactor", "Keep codebase clean and minimal", Priority::High, TaskStatus::Pending, "Work", "2026-09-05", {}, {} });
        manager.add({ 0, "Read C++ Documentation", "Standard library I/O and containers", Priority::Medium, TaskStatus::Pending, "Study", "", {}, {} });
    }

    bool running = true;
    while (running) {
        UI::clear();
        UI::printBanner();

        int total = 0, pending = 0, inProg = 0, done = 0, overdue = 0;
        manager.getStats(total, pending, inProg, done, overdue);
        UI::printStats(total, pending, inProg, done, overdue);

        std::cout << "  \033[1mMAIN MENU:\033[0m\n";
        std::cout << "  \033[36m1)\033[0m 📋 View & Search Tasks\n";
        std::cout << "  \033[36m2)\033[0m ➕ Add Task\n";
        std::cout << "  \033[36m3)\033[0m ✏️ Edit Task\n";
        std::cout << "  \033[36m4)\033[0m 🗑️ Delete Task\n";
        std::cout << "  \033[36m5)\033[0m 📝 Manage Subtasks & Notes\n";
        std::cout << "  \033[36m6)\033[0m ⚡ Bulk Operations (Clear Done, Mark All)\n";
        std::cout << "  \033[36m7)\033[0m 📄 Export to Markdown\n";
        std::cout << "  \033[31m0)\033[0m 🚪 Save & Exit\n\n";

        int choice = UI::getInt("Select option (0-7)", 0, 7, 1);

        switch (choice) {
            case 1: handleView(manager); break;
            case 2: handleAdd(manager); break;
            case 3: handleEdit(manager); break;
            case 4: handleDelete(manager); break;
            case 5: handleSubtasks(manager); break;
            case 6: handleBulk(manager); break;
            case 7: handleExport(manager); break;
            case 0:
                manager.save("tasks.dat");
                std::cout << "\n  \033[32m[✓] All tasks saved. Goodbye!\033[0m\n\n";
                running = false;
                break;
        }
    }

    return 0;
}
