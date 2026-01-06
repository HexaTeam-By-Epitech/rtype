/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SystemScheduler implementation
*/

#include "SystemScheduler.hpp"
#include <algorithm>
#include "../Logger/Logger.hpp"

namespace ecs::wrapper {

    SystemScheduler::SystemScheduler(ECSWorld *world) : _world(world), _needsReorder(false) {}

    SystemScheduler &SystemScheduler::registerSystem(const std::string &name, int priority) {
        SystemInfo info;
        info.name = name;
        info.priority = priority;
        info.enabled = true;

        _systemInfos[name] = info;
        _needsReorder = true;

        return *this;
    }

    SystemScheduler &SystemScheduler::runBefore(const std::string &systemName,
                                                const std::string &afterSystemName) {
        if (_systemInfos.find(systemName) == _systemInfos.end()) {
            LOG_ERROR("SystemScheduler::runBefore - System '", systemName, "' not registered");
            return *this;
        }

        _systemInfos[systemName].runBefore.push_back(afterSystemName);
        _needsReorder = true;

        return *this;
    }

    SystemScheduler &SystemScheduler::runAfter(const std::string &systemName,
                                               const std::string &beforeSystemName) {
        if (_systemInfos.find(systemName) == _systemInfos.end()) {
            LOG_ERROR("SystemScheduler::runAfter - System '", systemName, "' not registered");
            return *this;
        }

        _systemInfos[systemName].runAfter.push_back(beforeSystemName);
        _needsReorder = true;

        return *this;
    }

    void SystemScheduler::enable(const std::string &name) {
        auto it = _systemInfos.find(name);
        if (it != _systemInfos.end()) {
            it->second.enabled = true;
        }
    }

    void SystemScheduler::disable(const std::string &name) {
        auto it = _systemInfos.find(name);
        if (it != _systemInfos.end()) {
            it->second.enabled = false;
        }
    }

    bool SystemScheduler::isEnabled(const std::string &name) const {
        auto it = _systemInfos.find(name);
        if (it != _systemInfos.end()) {
            return it->second.enabled;
        }
        return false;
    }

    void SystemScheduler::computeExecutionOrder() {
        if (!_needsReorder) {
            return;
        }

        _executionOrder.clear();

        // Collect all system names
        for (const auto &[name, info] : _systemInfos) {
            _executionOrder.push_back(name);
        }

        // Sort by priority first (higher priority first)
        std::sort(_executionOrder.begin(), _executionOrder.end(),
                  [this](const std::string &a, const std::string &b) {
                      return _systemInfos[a].priority > _systemInfos[b].priority;
                  });

        // Apply topological sort for dependencies
        topologicalSort();

        _needsReorder = false;
    }

    void SystemScheduler::topologicalSort() {
        // Build dependency graph
        std::unordered_map<std::string, std::vector<std::string>> graph;
        std::unordered_map<std::string, int> inDegree;

        // Initialize
        for (const auto &name : _executionOrder) {
            graph[name] = {};
            inDegree[name] = 0;
        }

        // Build edges from dependencies
        for (const auto &[name, info] : _systemInfos) {
            // runBefore: name -> target
            for (const auto &target : info.runBefore) {
                if (_systemInfos.find(target) != _systemInfos.end()) {
                    graph[name].push_back(target);
                    inDegree[target]++;
                }
            }

            // runAfter: dependency -> name
            for (const auto &dependency : info.runAfter) {
                if (_systemInfos.find(dependency) != _systemInfos.end()) {
                    graph[dependency].push_back(name);
                    inDegree[name]++;
                }
            }
        }

        // Kahn's algorithm for topological sort
        std::vector<std::string> sorted;
        std::vector<std::string> queue;

        // Find all nodes with no incoming edges
        for (const auto &[name, degree] : inDegree) {
            if (degree == 0) {
                queue.push_back(name);
            }
        }

        // Sort queue by priority before processing
        std::sort(queue.begin(), queue.end(), [this](const std::string &a, const std::string &b) {
            return _systemInfos[a].priority > _systemInfos[b].priority;
        });

        while (!queue.empty()) {
            std::string current = queue.back();
            queue.pop_back();
            sorted.push_back(current);

            // Reduce in-degree of neighbors
            for (const auto &neighbor : graph[current]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    queue.push_back(neighbor);
                }
            }

            // Keep queue sorted by priority
            std::sort(queue.begin(), queue.end(), [this](const std::string &a, const std::string &b) {
                return _systemInfos[a].priority > _systemInfos[b].priority;
            });
        }

        // Check for cycles
        if (sorted.size() != _executionOrder.size()) {
            LOG_WARNING("Circular dependencies detected. Some systems may not execute in optimal order.");
        } else {
            _executionOrder = sorted;
        }
    }

    void SystemScheduler::update(float deltaTime) {
        if (_needsReorder) {
            computeExecutionOrder();
        }

        for (const auto &systemName : _executionOrder) {
            auto it = _systemInfos.find(systemName);
            if (it != _systemInfos.end() && it->second.enabled) {
                _world->updateSystem(systemName, deltaTime);
            }
        }
    }

    const std::vector<std::string> &SystemScheduler::getExecutionOrder() const {
        return _executionOrder;
    }

    void SystemScheduler::printExecutionOrder() const {
        std::cout << "=== System Execution Order ===" << std::endl;
        for (size_t i = 0; i < _executionOrder.size(); ++i) {
            const auto &name = _executionOrder[i];
            auto it = _systemInfos.find(name);
            if (it != _systemInfos.end()) {
                std::cout << (i + 1) << ". " << name << " (priority: " << it->second.priority
                          << ", enabled: " << (it->second.enabled ? "yes" : "no") << ")" << std::endl;
            }
        }
        std::cout << "==============================" << std::endl;
    }

}  // namespace ecs::wrapper
