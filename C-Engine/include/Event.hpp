//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_EVENT_HPP
#define C_ENGINE_EVENT_HPP

#include <string>
#include <any>
#include "ObjectList.hpp"

namespace engine {
    class Event {
    private:
        std::string _type;
        std::string _cause;

        engine::ObjectList<std::any> _data;
        engine::ObjectList<std::any> _response;

    public:

        Event(std::string what, std::string why) { _type = what; _cause = why; }

        /**
         * Returns the type of the event
         * @return The type of the event
         */
        std::string what() {
            return _type;
        }

        /**
         * Returns the cause of the event
         * @return the cause of the event
         */
        std::string why() {
            return _cause;
        }

        /**
         * Returns the type of the event as well as the cause seperated by a ':'
         * @return
         */
        std::string whole() {
            return _type + ": " + _cause;
        }

        /**
         * used to insert additional data to an event
         * @param ol an objectList containing all of the additional data
         */
        void setData(engine::ObjectList<std::any> &ol) {
            _data = ol;
        }

        /**
         * Returns additional data attached to the event
         * @return additional data attached to the event
         */
        engine::ObjectList<std::any> &getData() {
            return _data;
        }

        /**
         * NOT IMPLEMENTED
         */
        void setResponse(engine::ObjectList<std::any> &ol) {
            _response = ol;
        }

        /**
         * NOT IMPLEMENTED
         */
        engine::ObjectList<std::any> &getResponse() {
            return _data;
        }
    };
}

#endif //C_ENGINE_EVENT_HPP
