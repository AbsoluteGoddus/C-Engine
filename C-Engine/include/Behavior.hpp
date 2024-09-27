// Created by absol on 6/17/2024.

#ifndef C_ENGINE_BEHAVIOR_HPP
#define C_ENGINE_BEHAVIOR_HPP

#include <stdexcept>
#include <memory>
#include <utility>
#include "Event.hpp"

/**
 * @file Behavior.hpp
 * @brief Contains the Behavior class and BehaviorPTR class.
 */

namespace engine {

    /**
     * @class Behavior
     * @brief Base class for creating behaviors in the game engine.
     *
     * This class provides the necessary hooks for behaviors in a scene.
     * Derived classes can override these functions to define custom behavior.
     */
    class Behavior {
    public:
        /**
         * @fn void init()
         * @brief Called once at the start of the scene.
         *
         * This function is called when the scene in which the behavior is located is initialized.
         * Override it in derived classes to specify custom initialization logic.
         */
        virtual void init() {};

        /**
         * @fn void preUpdate()
         * @brief Called every tick before update() is called.
         *
         * This function is executed before each scene update. It can be used to prepare data or perform operations before the main update logic.
         *
         * @see update()
         */
        virtual void preUpdate() {};

        /**
         * @fn void update()
         * @brief Called every tick to update the behavior.
         *
         * This function is the main update routine and is called every frame to handle behavior-specific updates.
         *
         * @see preUpdate(), postUpdate()
         */
        virtual void update() {};

        /**
         * @fn void postUpdate()
         * @brief Called every tick after update() is called.
         *
         * This function is called after the main update logic has executed. Use this to perform any cleanup or final operations for the frame.
         *
         * @see update()
         */
        virtual void postUpdate() {};

        /**
         * @fn void onEvent(engine::Event e)
         * @brief Called when an event is triggered.
         *
         * This function is invoked whenever an event is triggered in the engine. It can handle various types of events like user inputs or Vulkan events.
         *
         * @param e The event that was triggered.
         */
        virtual void onEvent(engine::Event e) {};

        /**
         * @fn virtual ~Behavior()
         * @brief Called when the behavior is destroyed.
         *
         * Destructor for the Behavior class, called when the behavior is removed or when the scene it belongs to is destroyed.
         */
        virtual ~Behavior() {};
    };

    /**
     * @class BehaviorPTR
     * @brief A smart pointer wrapper to manage behaviors.
     *
     * This class manages behaviors using a `std::shared_ptr`. It allows setting, updating, and triggering events for behaviors in a managed and safe way.
     */
    class BehaviorPTR {
    private:
        std::shared_ptr<Behavior> _b = nullptr;  /**< Shared pointer to the managed Behavior. */

    public:
        /**
         * @brief Default constructor for the BehaviorPTR class.
         *
         * Initializes the BehaviorPTR with no behavior.
         */
        BehaviorPTR() : _b(nullptr) {}

        /**
         * @fn template<class BC> void setBehavior()
         * @tparam BC The type of the behavior to set.
         * @brief Sets the behavior to the specified type.
         *
         * This template function allows setting the internal behavior to a new behavior of type `BC`.
         */
        template<class BC>
        void setBehavior() {
            _b = std::make_shared<BC>();
        }

        /**
         * @fn void init()
         * @brief Calls the init function of the current behavior.
         *
         * If a behavior is set, this function invokes its `init()` function.
         */
        void init() {
            if (_b) _b->init();
        }

        /**
         * @fn void preUpdate()
         * @brief Calls the preUpdate function of the current behavior.
         *
         * If a behavior is set, this function invokes its `preUpdate()` function.
         */
        void preUpdate() {
            if (_b) _b->preUpdate();
        }

        /**
         * @fn void update()
         * @brief Calls the update function of the current behavior.
         *
         * If a behavior is set, this function invokes its `update()` function.
         */
        void update() {
            if (_b) _b->update();
        }

        /**
         * @fn void postUpdate()
         * @brief Calls the postUpdate function of the current behavior.
         *
         * If a behavior is set, this function invokes its `postUpdate()` function.
         */
        void postUpdate() {
            if (_b) _b->postUpdate();
        }

        /**
         * @fn bool isSet()
         * @brief Checks if a behavior is set.
         *
         * @return `true` if a behavior is currently set, otherwise `false`.
         */
        bool isSet() {
            return _b != nullptr;
        }

        /**
         * @fn void onEvent(engine::Event e)
         * @brief Calls the onEvent function of the current behavior.
         *
         * If a behavior is set, this function invokes its `onEvent()` function with the given event.
         *
         * @param e The event to handle.
         */
        void onEvent(engine::Event e) {
            if (_b) _b->onEvent(std::move(e));
        }
    };
}

#endif //C_ENGINE_BEHAVIOR_HPP
