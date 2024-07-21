//
// Created by absol on 7/2/2024.
//

#ifndef C_ENGINE_UTIL_HPP
#define C_ENGINE_UTIL_HPP

#include <string>

namespace engine {
    std::string remove(std::string &str, size_t start, size_t length) {
        std::string out = str.substr(start, length);
        std::string tmp = str.substr(start + length, str.size());
        str = str.substr(0, start);
        str += tmp;
        return out;
    }
}

namespace util {

    // Skip until a specific character is found
    void until(const std::string &f, size_t &pos, char c) {
        while (f[pos] != c) {
            pos++;
            if (pos >= f.size()) {
                pos = INT_MAX;
                break;
            }
        }
    }

    // Remove a substring from a string and return it
    std::string remove(std::string &f, size_t start, size_t length) {
        std::string out = f.substr(start, length);
        std::string tmp = f.substr(start + length, f.size());
        f = f.substr(0, start);
        f += tmp;
        return out;
    }

    void skipValueArray(std::string &f, size_t &pos) {
        size_t braceidx = 0;
        if (f[pos] == '{') {
            braceidx++;
            while (braceidx != 0) {
                pos++;
                if (pos >= f.size()) {
                    pos == INT_MAX;
                    return;
                }
                if (f[pos] == '{') {
                    braceidx++;
                }else if (f[pos] == '}') {
                    braceidx--;
                }
            }
        }
        pos++;
    }

    // Skip until the end of the current token
    void skipToken(const std::string &f, size_t &pos) {
        while (!std::isspace(f[pos]) && f[pos] != ':' && (f[pos] != '/' && f[pos + 1] != '*')) {
            pos++;
            if (pos >= f.size()) {
                break;
            }
        }
    }

    // Skip whitespace
    void skipEmpty(const std::string &f, size_t &pos) {
        while (std::isspace(f[pos])) {
            pos++;
            if (pos >= f.size()) {
                pos = INT_MAX;
                break;
            }
        }
    }

    // Skip comments in the form /* comment */
    void skipComment(const std::string &f, size_t &pos) {
        if (f[pos] == '/' && f[pos + 1] == '*') {
            int idx = pos + 2;
            for (size_t i = f.find('*', idx); i != std::string::npos; i = f.find('*', i + 1)) {
                if (f[i + 1] == '/') {
                    pos = i + 2;
                    return;
                }
            }
            pos = INT_MAX;
        }
    }

    std::vector<std::any> getValueArray(std::string &f) {
        std::string values = f.substr(1, f.size() - 2);
        std::vector<std::string> separateValues;
        std::vector<std::any> out;
        size_t pos = 0;
        // Seperate the values with ','
        pos = values.find(',');
        while (pos != std::string::npos) {
            separateValues.push_back(remove(values, 0, pos));
            util::remove(values, 0, 1);
            pos = values.find(',');
        }

        separateValues.push_back(values);

        int idx = 0;

        // Remove all spaces
        for (auto &s : separateValues){
            pos = s.find(' ');
            while (pos != std::string::npos) {
                remove(s, pos, 1);
                pos = s.find(' ');
            }
        }

        // Convert values from strings to their respective std::any forms
        for (auto &e : separateValues) {
            std::any tmp;
            std::string type = e.substr(0, e.find(':'));
            std::string val = e.substr(e.find(':') + 1, e.size() - e.find(':') - 1);

            if (type == "int") {
                tmp = std::stoi(val);
                out.push_back(tmp);
            } else if (type == "string" || (type[0] == '"' && type[type.size() - 1] == '"')) {
                tmp = val;
                out.push_back(tmp);
            } else if (type == "double") {
                tmp = std::stod(val);
                out.push_back(tmp);
            }
        }
        return out;
    }
}

#endif //C_ENGINE_UTIL_HPP
