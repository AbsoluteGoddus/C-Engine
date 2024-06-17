//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_OBJECTLIST_HPP
#define C_ENGINE_OBJECTLIST_HPP

#include <unordered_map>
#include <string>

namespace engine {
    template<class OL_Type>
    class ObjectList {
    private:
        std::unordered_map<std::string, int> _idMap;
        std::unordered_map<int, OL_Type> _list;

        int _nextID = 1;
    public:
        void addObject(OL_Type object, std::string name, int ID) {
            _idMap.insert(name, ID);
            _list.insert(ID, object);
        }

        void addObject(OL_Type object, int ID) {
            _idMap.insert(std::to_string(ID), ID);
            _list.insert(ID, object);
        }

        void addObject(OL_Type object, std::string name) {
            _idMap.insert(name, std::stoi(name));
            _list.insert(ID, object);
        }

        void removeObject(std::string name) {
            if (_idMap.find(name) != _idMap.end()) {
                int ID = _idMap.at(name);
                _list.erase(ID);
                _idMap.erase(name);
            }
        }

        void removeObject(int ID) {
            if (_list.find(ID) != _list.end()) {
                std::string name = std::to_string(ID);
                _list.erase(ID);
                _idMap.erase(name);
            }
        }

        OL_Type &operator[](const std::string &name) {
            auto it = _idMap.find(name);
            if (it != _idMap.end()) {
                return _list.at(it->second);
            }

            return nullptr;
        }

        OL_Type& operator[](int ID) {
            auto it = _list.find(ID);

            if (it != _list.end()) {
                return it->second;
            }

            return nullptr;
        }
    }

#endif //C_ENGINE_OBJECTLIST_HPP
