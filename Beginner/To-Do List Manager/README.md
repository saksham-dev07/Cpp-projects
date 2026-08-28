# 📋 TaskFlow - Modern C++ To-Do Manager (C++17)

[![CI Build & Test Matrix](https://github.com/saksham-dev07/C---projects/actions/workflows/ci.yml/badge.svg)](https://github.com/saksham-dev07/C---projects/actions)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/saksham-dev07/C---projects)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Dependencies](https://img.shields.io/badge/dependencies-0%20(Standard%20Library%20Only)-success.svg)](https://en.cppreference.com/)

A fast, modular, and lightweight **To-Do List & Task Management CLI** built in modern **C++ (C++17)**. Engineered following **Ponytail minimalism**—zero bloat, pure standard library, clean ANSI terminal UI, and instant file persistence.

---

## 🌟 Key Features

- **Full Task Lifecycle**: Track tasks across `Pending` `[ ]`, `In Progress` `[-]`, and `Completed` `[v]` statuses with automatic strike-through styling.
- **4 Priority Tiers**: Color-coded priority badges—`[LOW]` 🟢, `[MED]` 🟡, `[HIGH]` 🟠, and `[URGENT]` 🔴.
- **Checklist Subtasks**: Multi-item subtask checklists with dynamic percentage completion bars (e.g., `[====.] 4/5 (80%)`).
- **Smart Date Shortcuts & Overdue Flags**:
  - Natural date inputs: `today`, `tomorrow`, `+1d`, `+3d`, `+1w`, `+2w`, `+1m`, or raw `YYYY-MM-DD`.
  - Automatic `[!]` overdue indicators in tables and detail views.
- **Activity Notes**: Attach timestamped notes and comments to any task.
- **⚡ Bulk Operations**:
  - Clear all completed tasks in one click.
  - Mark all active tasks as completed.
  - Batch delete multiple tasks by comma-separated IDs (e.g., `1, 3, 7`).
- **📄 Markdown Export**: Export your task list, checklists, and notes to a clean GitHub-ready Markdown file (`todo_export.md`).
- **Instant Search & Multi-Sorting**: Filter by category (`@Work`, `@Dev`, `@Study`), search keywords in title/description/category, and sort by Due Date or Priority.
- **Pure Stdlib Persistence**: Fast, human-readable auto-save and restore using standard library file streams (`tasks.dat`).
- **Aesthetic Terminal UI**: Crisp box-drawing tables, colored status pills, summary statistics ribbon, and robust sanitized input prompts.

---

## 🏗️ Project Architecture

```
To-Do List Manager/
├── .github/
│   └── workflows/
│       └── ci.yml            # Multi-OS GitHub Actions CI Matrix (Ubuntu, Windows, macOS)
├── CMakeLists.txt            # Modern CMake build configuration (C++17)
├── LICENSE                   # MIT License
├── build.bat                 # Windows MSVC 1-click automated build script
├── build.sh                  # Linux / macOS shell compilation script
├── .gitignore                # Clean git ignore rules
├── README.md                 # Complete documentation & usage guide
├── src/
│   ├── Task.hpp              # Domain structs (Task, SubTask, Priority, Status) & date helpers (~60 lines)
│   ├── Storage.hpp/.cpp      # File persistence & Markdown export engine (~60 lines)
│   ├── TaskManager.hpp/.cpp  # Core CRUD, subtasks, notes, bulk ops & query filters (~120 lines)
│   ├── UI.hpp/.cpp           # ANSI terminal table rendering, badges & prompts (~120 lines)
│   └── main.cpp              # Interactive menu loop & controller (~120 lines)
└── tests/
    └── test_runner.cpp       # Automated unit test suite (42 assertions)
```

---

## 🚀 Building & Running

### Windows (MSVC Command Prompt / Batch)

Run the automated build script:
```cmd
build.bat
```

Run the application:
```cmd
bin\todo_manager.exe
```

Run the test suite:
```cmd
bin\todo_tests.exe
```

---

### CMake (Cross-Platform)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run main app
./todo_manager

# Run unit tests
ctest --output-on-failure
```

---

### Linux / macOS (g++ or clang++)

```bash
chmod +x build.sh
./build.sh

# Run main app
./bin/todo_manager

# Run unit tests
./bin/todo_tests
```

---

## ⌨️ Quick Shortcuts & Usage Guide

### Date Input Shortcuts
When prompted for a due date, enter standard `YYYY-MM-DD` or quick shortcuts:
- `today` / `t` $\rightarrow$ Current date
- `tomorrow` / `tom` $\rightarrow$ Tomorrow's date
- `+1d`, `+3d`, `+5d` $\rightarrow$ In $N$ days
- `+1w`, `+2w` $\rightarrow$ In $N$ weeks
- `+1m` $\rightarrow$ In 1 month

### Task Explorer Commands
- `[t]` **Toggle Status**: Quick progression (`Pending` $\rightarrow$ `In Progress` $\rightarrow$ `Done`)
- `[v]` **View Details**: Full metadata view with checklist progress and notes log
- `[n]` **Add Note**: Quickly attach a note or progress update to a task
- `[f]` **Filter**: Filter by category or search keyword in titles and descriptions
- `[s]` **Toggle Sort**: Instant switch between Due Date and Priority sorting
- `[q]` **Back**: Return to the main menu

---

## 🧪 Test Suite Results

The test suite covers all CRUD operations, subtasks, bulk operations, query filters, and file storage:

```text
=== Running Lean To-Do Tests (Ponytail) ===

Passed: 42 | Failed: 0
```

---

## 📄 License
Distributed under the MIT License. See [`LICENSE`](LICENSE) for more information.
