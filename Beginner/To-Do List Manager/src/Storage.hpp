#pragma once

#include "Task.hpp"
#include <string>
#include <vector>

namespace Todo {

    class Storage {
    public:
        static bool save(const std::string& filename, const std::vector<Task>& tasks);
        static bool load(const std::string& filename, std::vector<Task>& tasks);
        static bool exportMarkdown(const std::string& filename, const std::vector<Task>& tasks);
    };

} // namespace Todo
