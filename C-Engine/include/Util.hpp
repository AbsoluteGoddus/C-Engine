//
// Created by absol on 7/2/2024.
//

#ifndef C_ENGINE_UTIL_HPP
#define C_ENGINE_UTIL_HPP

#include <string>
#include <vector>
#include <cassert>
#include <any>

namespace engine {
    std::string remove(std::string &str, size_t start, size_t length) {
        std::string out = str.substr(start, length);
        std::string tmp = str.substr(start + length, str.size());
        str = str.substr(0, start);
        str += tmp;
        return out;
    }
}
#endif //C_ENGINE_UTIL_HPP
