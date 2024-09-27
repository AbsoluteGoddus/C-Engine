//
// Created by absol on 6/17/2024.
//

/**
 * \file Event.hpp
 * \brief Defines the Event class used for handling events in the engine.
 *
 * This file contains the definition of the Event class, which is used to represent and handle events within the engine.
 * Events can have a type, a cause, and additional data associated with them.
 */

#ifndef C_ENGINE_EVENT_HPP
#define C_ENGINE_EVENT_HPP

#include <string>
#include <any>
#include "ObjectList.hpp"

namespace engine {

    /**
     * \class Event
     * \brief Represents an event in the engine.
     *
     * The Event class is used to encapsulate information about events that occur within the engine.
     * Each event has a type, a cause, and can carry additional data.
     */
    class Event {
    private:
        std::string _type;  /**< The type of the event. */
        std::string _cause; /**< The cause of the event. */

        engine::ObjectList<std::any> _data;     /**< Additional data associated with the event. */
        engine::ObjectList<std::any> _response; /**< Response data for the event. */

    public:

        /**
         * \brief Constructs an Event with the specified type and cause.
         * \param what The type of the event.
         * \param why The cause of the event.
         */
        Event(std::string what, std::string why) { _type = what; _cause = why; }

        /**
         * \brief Returns the type of the event.
         * \return The type of the event.
         */
        std::string what() {
            return _type;
        }

        /**
         * \brief Returns the cause of the event.
         * \return The cause of the event.
         */
        std::string why() {
            return _cause;
        }

        /**
         * \brief Returns the type and cause of the event separated by a ':'.
         * \return A string containing the type and cause of the event.
         */
        std::string whole() {
            return _type + ": " + _cause;
        }

        /**
         * \brief Sets additional data for the event.
         * \param ol An ObjectList containing the additional data.
         */
        void setData(engine::ObjectList<std::any> &ol) {
            _data = ol;
        }

        /**
         * \brief Returns the additional data attached to the event.
         * \return A reference to the ObjectList containing the additional data.
         */
        engine::ObjectList<std::any> &getData() {
            return _data;
        }
    };
}

#endif //C_ENGINE_EVENT_HPP