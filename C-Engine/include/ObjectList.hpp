//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_OBJECTLIST_HPP
#define C_ENGINE_OBJECTLIST_HPP

#include <unordered_map>
#include <string>

namespace engine {
    /**
     * A simple manager to contain and manage objects.
     * @tparam OL_Type
     */
    template<class OL_Type>
    class ObjectList {
    private:
        std::unordered_map<std::string, int> _idMap;
        std::unordered_map<int, OL_Type> _list;

        int _nextID = 0;
    public:
        // Custom iterator class
        class Iterator {
        private:
            typename std::unordered_map<int, OL_Type>::iterator _it;
            typename std::unordered_map<int, OL_Type>::iterator _end;

        public:
            Iterator(typename std::unordered_map<int, OL_Type>::iterator it,
                     typename std::unordered_map<int, OL_Type>::iterator end)
                    : _it(it), _end(end) {}

            bool operator!=(const Iterator &other) const {
                return _it != other._it;
            }

            OL_Type &operator*() {
                return _it->second;
            }

            const Iterator &operator++() {
                ++_it;
                return *this;
            }
        };

        void addObject(const OL_Type &object) {
            int ID = _nextID++;
            _idMap[std::to_string(ID)] = ID;
            _list[ID] = object;
        }

        void addObject(const OL_Type &object, const std::string &name, int ID) {
            _idMap[name] = ID;
            _list[ID] = object;
        }

        void addObject(const OL_Type &object, int ID) {
            std::string name = std::to_string(ID);
            _nextID = ID + 1;
            _idMap[name] = ID;
            _list[ID] = object;
        }

        void addObject(const OL_Type &object, const std::string &name) {
            int ID = _nextID++;
            _idMap[name] = ID;
            _list[ID] = object;
        }

        void removeObject(const std::string &name) {
            auto it = _idMap.find(name);
            if (it != _idMap.end()) {
                int ID = it->second;
                _list.erase(ID);
                _idMap.erase(it);
            }
        }

        void removeObject(int ID) {
            std::string name = std::to_string(ID);
            auto it = _list.find(ID);
            if (it != _list.end()) {
                _list.erase(it);
                _idMap.erase(name);
            }
        }

        OL_Type *getObject(const std::string &name) {
            auto it = _idMap.find(name);
            if (it != _idMap.end()) {
                return &_list[it->second];
            }
            return nullptr;
        }

        OL_Type *getObject(int ID) {
            auto it = _list.find(ID);
            if (it != _list.end()) {
                return &it->second;
            }
            return nullptr;
        }

        size_t size() const {
            return _list.size();
        }

        Iterator begin() {
            return Iterator(_list.begin(), _list.end());
        }

        Iterator end() {
            return Iterator(_list.end(), _list.end());
        }

        OL_Type *operator[](int ID) {
            return getObject(ID);
        }

        OL_Type *operator[](std::string name) {
            return getObject(name);
        }

        OL_Type &operator()(int ID) {
            return getObject(ID);
        }

        OL_Type &operator()(std::string name) {
            return getObject(name);
        }

    };
}

#endif //C_ENGINE_OBJECTLIST_HPP