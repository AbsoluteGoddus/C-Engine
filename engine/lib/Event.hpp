//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_EVENT_HPP
#define C_ENGINE_EVENT_HPP

#include <string>

namespace engine {
    class Event {
    private:
        std::string _type;
        std::string _cause;

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
    };
}

#endif //C_ENGINE_EVENT_HPP
