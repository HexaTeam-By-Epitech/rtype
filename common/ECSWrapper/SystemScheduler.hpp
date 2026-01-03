/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SystemScheduler - Advanced system execution control
*/

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ECSWorld.hpp"

namespace ecs::wrapper {

    /**
     * @class SystemScheduler
     * @brief Advanced scheduler for controlling system execution order and dependencies.
     * 
     * Allows defining execution stages, priorities, and dependencies between systems.
     * Useful for complex server-side game logic with specific ordering requirements.
     */
    class SystemScheduler {
       public:
        /**
         * @struct SystemInfo
         * @brief Information about a scheduled system.
         */
        struct SystemInfo {
            std::string name;
            int priority;                        ///< Higher priority systems run first
            std::vector<std::string> runBefore;  ///< Systems that must run after this one
            std::vector<std::string> runAfter;   ///< Systems that must run before this one
            bool enabled;
        };

       private:
        ECSWorld *_world;
        std::unordered_map<std::string, SystemInfo> _systemInfos;
        std::vector<std::string> _executionOrder;
        bool _needsReorder;

        void computeExecutionOrder();
        void topologicalSort();

       public:
        /**
         * @brief Construct a SystemScheduler.
         * 
         * @param world Pointer to the ECS world
         */
        explicit SystemScheduler(ECSWorld *world);

        /**
         * @brief Register a system with the scheduler.
         * 
         * @param name System name
         * @param priority Execution priority (higher = earlier)
         * @return SystemScheduler& Reference for chaining
         */
        SystemScheduler &registerSystem(const std::string &name, int priority = 0);

        /**
         * @brief Specify that a system must run before another.
         * 
         * @param systemName The system that runs first
         * @param afterSystemName The system that runs after
         * @return SystemScheduler& Reference for chaining
         */
        SystemScheduler &runBefore(const std::string &systemName, const std::string &afterSystemName);

        /**
         * @brief Specify that a system must run after another.
         * 
         * @param systemName The system that runs second
         * @param beforeSystemName The system that runs before
         * @return SystemScheduler& Reference for chaining
         */
        SystemScheduler &runAfter(const std::string &systemName, const std::string &beforeSystemName);

        /**
         * @brief Enable a system.
         * 
         * @param name System name
         */
        void enable(const std::string &name);

        /**
         * @brief Disable a system.
         * 
         * @param name System name
         */
        void disable(const std::string &name);

        /**
         * @brief Check if a system is enabled.
         * 
         * @param name System name
         * @return bool true if enabled, false otherwise
         */
        bool isEnabled(const std::string &name) const;

        /**
         * @brief Update all enabled systems in the scheduled order.
         * 
         * @param deltaTime Time elapsed since last update
         */
        void update(float deltaTime);

        /**
         * @brief Get the computed execution order.
         * 
         * @return const std::vector<std::string>& Vector of system names in execution order
         */
        const std::vector<std::string> &getExecutionOrder() const;

        /**
         * @brief Print the execution order for debugging.
         */
        void printExecutionOrder() const;
    };

}  // namespace ecs::wrapper
