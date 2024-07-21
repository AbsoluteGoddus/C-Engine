// CFG.hpp

#ifndef CFG_HPP
#define CFG_HPP

#include <iostream>

#include "Util.hpp"
#include "Node.hpp"

class CFG {
//private:
public:
    std::string cleanedData;

    /*
     * Cleans up Human-Readable .CFG code into .CFG-Intermediate code.
     */
    void cleanup(std::string &f) {
        size_t pos = 0;
        size_t tmp = 0;
        bool curr_var = false;
        int tkidx = 0;
        while (pos < f.size()) {
            pos = 0;
            if (std::isspace(f[pos])) {
                tmp = pos;
                util::skipEmpty(f, pos);
                util::remove(f, tmp, pos - tmp);
                pos = tmp;
            } else if (f[pos] == '/' && f[pos + 1] == '*') {
                tmp = pos;
                util::skipComment(f, pos);
                util::remove(f, tmp, pos - tmp);
                pos = tmp;
            } else if ( tkidx >= 2 || (f[pos] != '{' && f[pos] != '}')){
                if (tkidx < 2) {
                    tmp = pos;
                    util::skipToken(f, pos);
                    std::string stmp = util::remove(f, tmp, pos - tmp);
                    if (f[0] == ':') {
                        util::remove(f, 0, 1);
                        if (tkidx != 0) {
                            cleanedData += '$';
                        } else {
                            cleanedData += '%';
                        }
                    }
                    if (stmp == "$") {
                        curr_var = true;
                    } else {
                        cleanedData += stmp + ' ';
                    }
                } else {
                    tmp = pos;
                    util::skipValueArray(f, pos);
                    cleanedData += util::remove(f, tmp, pos - tmp) + ' ';
                    curr_var = false;
                }
                if (curr_var) {
                    tkidx++;
                } else {
                    tkidx = 0;
                }
            } else if (f[pos] == '}') {
                util::remove(f, 0, 1);
                cleanedData += "# ";
            } else if (f[pos] == '{') {
                util::remove(f, 0, 1);
            }
        }
    }

    /*
     * Parses .CFG-Intermediate code into a Node
     */
    void parse(std::string &f, Node *root) {
        nodeValue tmpVal;
        size_t pos = 0;
        size_t tmp = 0;
        std::string token;
        Node *currentNode = root;
        bool isValue = false;
        while (pos < f.size() - 1) {
            // Get the Token
            if (!isValue) {
                /*
                * Explanation: Cleanup always puts a space, and then a token, so we can MOSTLY extract it very easily
                */
                util::skipEmpty(f, pos);
                tmp = pos;
                util::skipToken(f, pos);
                token = f.substr(tmp, pos - tmp);
            }else {
                /*
                 * Explanation: If we find that this token is a value / array beforehand, we can simply extract it without other shenanigans.
                 */
                util::skipEmpty(f, pos);
                tmp = pos;
                util::skipValueArray(f, pos);

                // Push tempVal into root
                std::string _tmp = f.substr(tmp, pos - tmp);
                tmpVal.second = util::getValueArray(_tmp);

                *currentNode < tmpVal;
            }
            if (!isValue) {
                if (token[0] == '$') {
                    tmpVal.first = token.substr(1, token.size() - 1);
                    isValue = true;
                }else if (token[0] == '%') {
                    std::string nName = token.substr(1, token.size() - 1);
                    currentNode = &(*currentNode)[nName];
                }else if (token[0] == '#') {
                    currentNode = currentNode->getParent();
                }
            }else {
                isValue = false;
            }
        }
    }

public:
};

#endif //CFG_HPP
