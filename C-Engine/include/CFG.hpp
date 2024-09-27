#ifndef CFG_HPP
#define CFG_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Node.hpp"

/**
 * \file CFG.hpp
 * \brief Defines functions for parsing and serializing .cfg files.
 */

namespace engine {
    /**
     * @brief Gets a token
     *
     * Gets a token from a string str. the token ends if the function finds a space.
     *
     * @param str The string to process
     * @param pos The starting position
     * @return The end of the Token
     */
    size_t getToken(const std::string& str, size_t pos) {
        size_t idx = pos;
        while (!std::isspace(str[idx])) {
            idx++;
        }
        return idx;
    }

    std::string getTokenStr(const std::string& str, size_t pos) {
        std::string buffer;

        size_t idx = pos;

        while (!std::isspace(str[idx])) {
            buffer += str[idx];
            idx++;
        }

        return buffer;
    }

    size_t getScope(const std::string& str, size_t pos) {
        size_t idx = pos;

        if (str[pos] != '{') {
            throw std::runtime_error("Unable to search scope: Unknown start\nCFG.hpp:46");
        }

        size_t scopeIdx = 1;

        while (scopeIdx != 0) {
            if (str[++idx] == '}') {
                scopeIdx--;
            } else if (str[idx] == '{') {
                scopeIdx++;
            }
        }

        return idx;
    }

    std::string getScopeStr(const std::string &str, size_t pos) {
        std::string buffer;

        size_t idx = pos;

        if (str[pos] != '{') {
            std::cerr << str[pos];
            throw std::runtime_error("Unable to search scope: Unknown start\nCFG.hpp:69");
        }

        size_t scopeIdx = 1;

        while (scopeIdx != 0) {
            buffer += str[idx];
            if (str[++idx] == '}') {
                scopeIdx--;
            } else if (str[idx] == '{') {
                scopeIdx++;
            }
        }

        buffer += str[idx];

        return buffer;
    }

    std::vector<std::any> getValuesFromArray(const std::string& str) {
        return {str};
    };

    std::vector<std::any> processArray(const std::string& str) {
        std::vector<std::any> values;
        std::string pureArray = str.substr(1, str.size() - 1).substr(0, str.size() - 2);

        std::string buffer;
        std::string valType;

        size_t ptr = 0;

        while (ptr < pureArray.size()) {
            char c = pureArray[ptr];

            if (c == '\"') {
                ptr++;
                while (pureArray[ptr] != '\"' || pureArray[ptr - 1] == ':') {
                    buffer += (pureArray[ptr] == ':' && pureArray[ptr + 1] == '\"') ? '\"' : pureArray[ptr];
                    ptr++;
                }
                values.emplace_back(buffer);
                buffer.clear();
                ptr += 3;
            } else {
                while (pureArray[ptr] != ':') {
                    valType += pureArray[ptr];
                    ptr++;
                }
                ptr += 2;

                while (!std::isspace(pureArray[ptr]) && pureArray[ptr] != ',' && pureArray[ptr] != '}' && pureArray[ptr] != '\000' && pureArray[ptr] != '\253') {
                    buffer += pureArray[ptr];
                    ptr++;
                }
                ptr += 2;

                if (valType == "int") {
                    values.emplace_back(std::stoi(buffer));
                } else if (valType == "double") {
                    values.emplace_back(std::stod(buffer));
                } else if (valType == "bool") {
                    bool tmp = (buffer == "true");
                    values.emplace_back(tmp);
                }
                valType.clear();
                buffer.clear();
            }
        }

        return values;
    }

    std::string anyArray_to_stringArray(const std::vector<std::any>& v) {
        std::string buffer = "{";
        bool first = true;

        for (const auto& a : v) {
            if (a.type() == typeid(std::string)) {
                buffer += (first) ? "" : ", ";
                buffer += "\"" + std::any_cast<std::string>(a) + "\"";
                first = false;
            } else if (a.type() == typeid(int)) {
                buffer += (first) ? "" : ", ";
                buffer += "int: " + std::to_string(std::any_cast<int>(a));
                first = false;
            } else if (a.type() == typeid(double)) {
                buffer += (first) ? "" : ", ";
                buffer += "double: " + std::to_string(std::any_cast<double>(a));
                first = false;
            } else if (a.type() == typeid(bool)) {
                buffer += (first) ? "" : ", ";
                buffer += "bool: " + std::to_string(std::any_cast<bool>(a));
                first = false;
            }
        }

        buffer += '}';

        return buffer;
    }

    /**
     * Parses .cfg code into a node
     * @param str The .cfg code to be parsed
     * @param n The node in which the output should be placed
     */
    void cfg_parse(const std::string& str, Node& n) {
        Node *curr = &n;
        nodeValue tmp;

        for (size_t i = 0; i < str.size(); ++i) {
            char c = str[i];

            // Variable
            if (c == '$') {
                std::string valName = getTokenStr(str, i + 2);
                valName = valName.substr(0, valName.size() - 1);
                tmp = nodeValue(valName, processArray(getScopeStr(str, getToken(str, i + 2) + 1)));
                curr->pushValue(tmp.first, tmp.second);
                i = getScope(str, getToken(str, i + 2) + 1);
                continue;
            } else if (c == '}') {
                curr = curr->getParent();
            } else if (c == '{') {
              continue;
            } else if (!std::isspace(c)) {
                std::string scopeName = getTokenStr(str, i);
                scopeName = scopeName.substr(0, scopeName.size() - 1);
                curr = &(*curr)[scopeName];
                i = getToken(str, i);
            }
        }
    }

    /**
     * Makes a string out of chars
     * @param c the char to be chained together to a string
     * @param times How many times the char should be chained together
     * @return The resulting string
     */
    std::string charString(const char& c, size_t times) {
        std::string out;
        for (int i = 0; i < times; ++i) {
            out += c;
        }
        return out;
    }

    /**
     * Serializes a Node-tree into a string containing .cfg code
     * @param root The root of the Node-tree to be serialized
     * @return The serialized .cfg code
     */
    std::string cfg_serialize(const Node& root) {
        std::string out = root.getName() + ": { ";
        for (const auto& v : root.getValues()) {
            out += "$ " + v.first + ": " + anyArray_to_stringArray(std::any_cast<std::vector<std::any>>(v.second)) + " ";
        }
        for (const auto &n : root.getSubNodes()) {
            out += cfg_serialize(n.second);
        }
        out += "} ";
        return out;
    }

    /**
     * Serializes a Node-tree into a string containing .cfg code. Also indents the string and inserts newlines to make the string more Human-readable
     * @param root The root of the Node-tree to be serialized
     * @param indents The number of tas te current level should put before it.
     * @return The serialized .cfg code
     */
    std::string cfg_serialize(const Node& root, const size_t& indents) {
        std::string out = charString('\t', indents) + root.getName() + ": {\n";
        for (const auto& v : root.getValues()) {
            out += charString('\t', indents + 1) + "$ " + v.first + ": " + anyArray_to_stringArray(std::any_cast<std::vector<std::any>>(v.second)) + "\n";
        }
        for (const auto &n : root.getSubNodes()) {
            out += cfg_serialize(n.second, indents + 1);
        }
        out += charString('\t', indents) + "}\n";
        return out;
    }

    /**
     * Serializes a Node-tree into a string containing .cfg code. Also indents the string and inserts newlines to make the string more Human-readable. Does not include the root node in the output
     * @param root The root of the Node-tree to be serialized
     * @param indents The number of tas te current level should put before it.
     * @return The serialized .cfg code
     */
    std::string cfg_serialize_(const Node& root, const size_t& indents) {
        std::string out;
        for (const auto& v : root.getValues()) {
            out += charString('\t', indents) + "$ " + v.first + ": " + anyArray_to_stringArray(std::any_cast<std::vector<std::any>>(v.second)) + "\n";
        }
        for (const auto &n : root.getSubNodes()) {
            out += cfg_serialize(n.second, indents);
        }
        return out;
    }

     /**
      * Serializes a Node-tree into a string containing .cfg code. Does not include the root node in the output
      * @param root The root of the Node-tree to be serialized
      * @return The serialized .cfg code
      */
     std::string cfg_serialize_(const Node& root) {
        std::string out;
         for (const auto &v: root.getValues()) {
             out += "$ " + v.first + ": " + anyArray_to_stringArray(std::any_cast<std::vector<std::any>>(v.second)) + " ";
         }
         for (const auto &n: root.getSubNodes()) {
             out += cfg_serialize(n.second);
         }
         return out;
     }
}

#endif // CFG_HPP