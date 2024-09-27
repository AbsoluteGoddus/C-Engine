#ifndef TASK_TASK_HPP
#define TASK_TASK_HPP

#include <any>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <functional>
#include <vector>
#include <mutex>

/**
 * \file task.hpp
 * \brief Defines the task class used for representing tasks that can be executed.
 */

/**
 * \class task
 * \brief Represents a task that can be executed, with dependencies on other tasks.
 *
 * The task class encapsulates a unit of work that can be executed. Each task can have dependencies on other tasks,
 * and it can be run either sequentially or in parallel with its dependencies.
 */
class task {
private:
    int tID; /**< The ID of the task. */
    std::string tName; /**< The name of the task. */
    std::function<void(task&, std::any)> tFunction; /**< The function to be executed by the task. */
    std::vector<task*> requiredTasks; /**< Tasks required to be executed before this task can run. */
    int status = 0; /**< The status of the task: 0 = Idle, 1 = Running, 2 = Finished. */
    task* parentTask = nullptr; /**< Pointer to the parent task. */
    std::any tArgs; /**< Arguments for the task function. */
    std::unordered_map<std::string, std::pair<std::any, std::any>> childArgs; /**< Arguments for child tasks. */
    std::string toutProcess; /**< Process information for logging. */
    std::mutex toutMutex; /**< Mutex for thread safety. */

public:
    /**
     * \brief Gets the arguments for child tasks.
     * \return A map of child task arguments.
     */
    auto getChildArgs() {
        return childArgs;
    }

    /**
     * \brief Gets the current process information for logging.
     * \return The current process information.
     */
    const std::string& getToutProcess() {
        return toutProcess;
    }

    /**
     * \brief Sets the current process information for logging.
     * \param process The process information to set.
     */
    void setToutProcess(const std::string& process) {
        toutProcess = process;
    }

    // Getters

    /**
     * \brief Gets the ID of the task.
     * \return The ID of the task.
     */
    int getID() const {
        return tID;
    }

    /**
     * \brief Gets the name of the task.
     * \return The name of the task.
     */
    const std::string& getName() const {
        return tName;
    }

    /**
     * \brief Gets the function to be executed by the task.
     * \return The function to be executed by the task.
     */
    const std::function<void(task&, std::any)>& getFunction() const {
        return tFunction;
    }

    /**
     * \brief Gets the tasks required to be executed before this task can run.
     * \return A vector of required tasks.
     */
    const std::vector<task*>& getRequiredTasks() const {
        return requiredTasks;
    }

    /**
     * \brief Gets the status of the task.
     * \return The status of the task.
     */
    int getStatus() const {
        return status;
    }

    /**
     * \brief Gets the parent task.
     * \return A pointer to the parent task.
     */
    task* getParent() const {
        return parentTask;
    }

    /**
     * \brief Gets the arguments for the task function.
     * \return The arguments for the task function.
     */
    const std::any& getArgs() const {
        return tArgs;
    }

    // Setters

    /**
     * \brief Sets the ID of the task.
     * \param id The ID to set.
     */
    void setID(int id) {
        tID = id;
    }

    /**
     * \brief Sets the name of the task.
     * \param name The name to set.
     */
    void setName(const std::string& name) {
        tName = name;
    }

    /**
     * \brief Sets the function to be executed by the task.
     * \param function The function to set.
     */
    void setFunction(const std::function<void(task&, std::any)>& function) {
        tFunction = function;
    }

    /**
     * \brief Sets the tasks required to be executed before this task can run.
     * \param tasks A vector of required tasks.
     */
    void setRequiredTasks(const std::vector<task*>& tasks) {
        requiredTasks = tasks;
        for (auto &task : requiredTasks) {
            task->setParent(*this);
        }
    }

    /**
     * \brief Sets the status of the task.
     * \param nStatus The status to set.
     */
    void setStatus(int nStatus) {
        status = nStatus;
    }

    /**
     * \brief Sets the parent task.
     * \param parent The parent task to set.
     */
    void setParent(task& parent) {
        parentTask = &parent;
    }

    /**
     * \brief Sets the arguments for the task function.
     * \param args The arguments to set.
     */
    void setArgs(const std::any& args) {
        tArgs = args;
    }

    /**
     * \brief Logs a message with the task's name and process information.
     * \param message The message to log.
     */
    void tout(const std::string& message) {
        std::lock_guard<std::mutex> lock(toutMutex); // Lock the mutex for thread safety
        std::string fullMessage = "[" + tName + "] " + toutProcess + message;
        if (parentTask != nullptr) {
            parentTask->tout(fullMessage);
        } else {
            std::cout << fullMessage << "\n";
        }
    }

    /**
     * \brief Runs the task and its required tasks sequentially.
     *
     * This method runs the task and ensures that all required tasks are completed before running the main task.
     * It logs the progress and execution time of the task.
     */
    void run() {
        auto rootStart = std::chrono::high_resolution_clock::now();
        status = 1;
        toutProcess = "[GetRequiredTasks] ";
        for (auto &requiredTask : requiredTasks) {
            tout("Checking required task: " + requiredTask->tName + "...");
            if (requiredTask->getStatus() == 0) {
                tout("Required task not started: " + requiredTask->tName);
                tout("Starting required task: " + requiredTask->tName + "...");
                requiredTask->run();
                auto start = std::chrono::high_resolution_clock::now();
                while (requiredTask->getStatus() != 2) {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> elapsed = end - start;
                    tout("Waiting for required task to finish: " + requiredTask->tName + " | Elapsed time: " + std::to_string(elapsed.count()) + " ms...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            } else if (requiredTask->getStatus() == 1) {
                auto start = std::chrono::high_resolution_clock::now();
                while (requiredTask->getStatus() != 2) {
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> elapsed = end - start;
                    tout("Waiting for required task to finish: " + requiredTask->tName + " | Elapsed time: " + std::to_string(elapsed.count()) + " ms...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            tout("Required task finished: " + requiredTask->tName);
            tout("Adding task args to child args...");
            childArgs[requiredTask->tName] = {requiredTask->getChildArgs(), requiredTask->getArgs()};
            tout("Finished adding task args to child args.");
        }
        toutProcess = "[Run] ";

        tout("Running task...");
        tFunction(*this, tArgs);
        status = 2;
        auto rootEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = rootEnd - rootStart;
        tout("Task finished in " + std::to_string(elapsed.count()) + " ms.");
        toutProcess = "";
    }

    /**
     * \brief Runs the task and its required tasks in parallel.
     *
     * This method runs the task and its required tasks in separate threads, allowing for parallel execution.
     */
    void tRun() {
        std::vector<std::thread> threads;
        for (auto &requiredTask : requiredTasks) {
            threads.emplace_back(&task::tRun, requiredTask);
        }
        run();
        for (auto &thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
};

#endif // TASK_TASK_HPP