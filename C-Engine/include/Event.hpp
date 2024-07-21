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

        std::string what() {
            return _type;
        }

        std::string why() {
            return _cause;
        }

        std::string whole() {
            return _type + ": " + _cause;
        }

        void setData(engine::ObjectList<std::any> &ol) {
            _data = ol;
        }

        engine::ObjectList<std::any> &getData() {
            return _data;
        }

        void setResponse(engine::ObjectList<std::any> &ol) {
            _response = ol;
        }

        engine::ObjectList<std::any> &getResponse() {
            return _data;
        }
    };
}

#endif //C_ENGINE_EVENT_HPP
