//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_GAMEOBJECT_HPP
#define C_ENGINE_GAMEOBJECT_HPP

#include "ObjectList.hpp"
#include "Behavior.hpp"

#ifdef C_ENGINE_USE_THREADS
#include <thread>
#include <functional>
#include <vector>
#endif

/**
 * \file GameObject.hpp
 * \brief Defines the GameObject class used for containing and executing Behaviors.
 */

namespace engine {
    /**
     * \class GameObject
     * \brief Class for containing and executing Behaviors.
     *
     * The GameObject class manages a list of behaviors and provides methods to initialize, update, and handle events for these behaviors.
     * It supports both single-threaded and multi-threaded execution based on the \c C_ENGINE_USE_THREADS macro.
     */
    class GameObject {
    private:
    public:
        ObjectList<BehaviorPTR> Behaviors; /**< List of behaviors associated with the GameObject. */

#ifndef C_ENGINE_USE_THREADS
        /**
         * \brief Initializes all behaviors.
         */
        void init() {
            for (auto &b : Behaviors) {
                b.onEvent(Event("GetGameObj", "GameObject initialized.", ObjectList<std::any>{this}));
            }

            for (auto &b : Behaviors) {
                b.init();
            }
        }

        /**
         * \brief Pre-update step for all behaviors.
         */
        void preUpdate() {
            for (auto &b : Behaviors) {
                b.preUpdate();
            }
        }

        /**
         * \brief Updates all behaviors.
         */
        void update() {
            for (auto &b : Behaviors) {
                b.update();
            }
        }

        /**
         * \brief Post-update step for all behaviors.
         */
        void postUpdate() {
            for (auto &b : Behaviors) {
                b.postUpdate();
            }
        }

        /**
         * \brief Handles an event for all behaviors.
         * \param e The event to handle.
         */
        void onEvent(engine::Event &e) {
            for (auto &b : Behaviors) {
                b.onEvent(e);
            }
        }
#else
        /**
         * \brief Initializes all behaviors in parallel.
         */
        void init() {
            std::vector<std::thread> threads;
            for (auto &b : Behaviors) {
                threads.emplace_back([&b]() { b.init(); });
            }
            for (auto &t : threads) {
                t.join();
            }
        }

        /**
         * \brief Pre-update step for all behaviors in parallel.
         */
        void preUpdate() {
            std::vector<std::thread> threads;
            for (auto &b : Behaviors) {
                threads.emplace_back([&b]() { b.preUpdate(); });
            }
            for (auto &t : threads) {
                t.join();
            }
        }

        /**
         * \brief Updates all behaviors in parallel.
         */
        void update() {
            std::vector<std::thread> threads;
            for (auto &b : Behaviors) {
                threads.emplace_back([&b]() { b.update(); });
            }
            for (auto &t : threads) {
                t.join();
            }
        }

        /**
         * \brief Post-update step for all behaviors in parallel.
         */
        void postUpdate() {
            std::vector<std::thread> threads;
            for (auto &b : Behaviors) {
                threads.emplace_back([&b]() { b.postUpdate(); });
            }
            for (auto &t : threads) {
                t.join();
            }
        }

        /**
         * \brief Handles an event for all behaviors in parallel.
         * \param e The event to handle.
         */
        void onEvent(engine::Event &e) {
            std::vector<std::thread> threads;
            for (auto &b : Behaviors) {
                threads.emplace_back([&b, &e]() { b.onEvent(e); });
            }
            for (auto &t : threads) {
                t.join();
            }
        }
#endif
    };
}

#endif //C_ENGINE_GAMEOBJECT_HPP