//
// Created by absol on 6/17/2024.
//

/**
 * \file ObjectList.hpp
 * \brief Defines the ObjectList class template used for managing objects in the engine.
 *
 * This file contains the definition of the ObjectList class template, which is used to manage a collection of objects.
 * Each object can be identified by a unique ID or a name.
 */

#ifndef C_ENGINE_OBJECTLIST_HPP
#define C_ENGINE_OBJECTLIST_HPP

#include <unordered_map>
#include <string>

namespace engine {

    /**
     * \class ObjectList
     * \brief A simple manager to contain and manage objects.
     * \tparam OL_Type The type of objects to be managed.
     *
     * The ObjectList class template is used to manage a collection of objects. Each object can be identified by a unique ID or a name.
     */
    template<class OL_Type>
    class ObjectList {
    private:
        std::unordered_map<std::string, int> _idMap; /**< Maps object names to their IDs. */
        std::unordered_map<int, OL_Type> _list;      /**< Maps object IDs to the objects. */

        int _nextID = 0; /**< The next available ID for a new object. */
    public:
        /**
         * \class Iterator
         * \brief Custom iterator class for ObjectList.
         *
         * The Iterator class is used to iterate over the objects in the ObjectList.
         */
        class Iterator {
        private:
            typename std::unordered_map<int, OL_Type>::iterator _it; /**< Iterator to the current object. */
            typename std::unordered_map<int, OL_Type>::iterator _end; /**< Iterator to the end of the list. */

        public:
            /**
             * \brief Constructs an Iterator.
             * \param it Iterator to the current object.
             * \param end Iterator to the end of the list.
             */
            Iterator(typename std::unordered_map<int, OL_Type>::iterator it,
                     typename std::unordered_map<int, OL_Type>::iterator end)
                    : _it(it), _end(end) {}

            /**
             * \brief Checks if two iterators are not equal.
             * \param other The other iterator to compare with.
             * \return `true` if the iterators are not equal, otherwise `false`.
             */
            bool operator!=(const Iterator &other) const {
                return _it != other._it;
            }

            /**
             * \brief Dereferences the iterator to access the object.
             * \return A reference to the object.
             */
            OL_Type &operator*() {
                return _it->second;
            }

            /**
             * \brief Advances the iterator to the next object.
             * \return A reference to the updated iterator.
             */
            const Iterator &operator++() {
                ++_it;
                return *this;
            }
        };

        /**
         * \brief Adds an object to the list with an automatically assigned ID.
         * \param object The object to add.
         */
        void addObject(const OL_Type &object) {
            int ID = _nextID++;
            _idMap[std::to_string(ID)] = ID;
            _list[ID] = object;
        }

        /**
         * \brief Adds an object to the list with a specified name and ID.
         * \param object The object to add.
         * \param name The name of the object.
         * \param ID The ID of the object.
         */
        void addObject(const OL_Type &object, const std::string &name, int ID) {
            _idMap[name] = ID;
            _list[ID] = object;
        }

        /**
         * \brief Adds an object to the list with a specified ID.
         * \param object The object to add.
         * \param ID The ID of the object.
         */
        void addObject(const OL_Type &object, int ID) {
            std::string name = std::to_string(ID);
            _nextID = ID + 1;
            _idMap[name] = ID;
            _list[ID] = object;
        }

        /**
         * \brief Adds an object to the list with a specified name.
         * \param object The object to add.
         * \param name The name of the object.
         */
        void addObject(const OL_Type &object, const std::string &name) {
            int ID = _nextID++;
            _idMap[name] = ID;
            _list[ID] = object;
        }

        /**
         * \brief Removes an object from the list by its name.
         * \param name The name of the object to remove.
         */
        void removeObject(const std::string &name) {
            auto it = _idMap.find(name);
            if (it != _idMap.end()) {
                int ID = it->second;
                _list.erase(ID);
                _idMap.erase(it);
            }
        }

        /**
         * \brief Removes an object from the list by its ID.
         * \param ID The ID of the object to remove.
         */
        void removeObject(int ID) {
            std::string name = std::to_string(ID);
            auto it = _list.find(ID);
            if (it != _list.end()) {
                _list.erase(it);
                _idMap.erase(name);
            }
        }

        /**
         * \brief Retrieves an object from the list by its name.
         * \param name The name of the object to retrieve.
         * \return A pointer to the object, or `nullptr` if not found.
         */
        OL_Type *getObject(const std::string &name) {
            auto it = _idMap.find(name);
            if (it != _idMap.end()) {
                return &_list[it->second];
            }
            return nullptr;
        }

        /**
         * \brief Retrieves an object from the list by its ID.
         * \param ID The ID of the object to retrieve.
         * \return A pointer to the object, or `nullptr` if not found.
         */
        OL_Type *getObject(int ID) {
            auto it = _list.find(ID);
            if (it != _list.end()) {
                return &it->second;
            }
            return nullptr;
        }

        /**
         * \brief Returns the number of objects in the list.
         * \return The number of objects in the list.
         */
        size_t size() const {
            return _list.size();
        }

        /**
         * \brief Returns an iterator to the beginning of the list.
         * \return An iterator to the beginning of the list.
         */
        Iterator begin() {
            return Iterator(_list.begin(), _list.end());
        }

        /**
         * \brief Returns an iterator to the end of the list.
         * \return An iterator to the end of the list.
         */
        Iterator end() {
            return Iterator(_list.end(), _list.end());
        }

        /**
         * \brief Retrieves an object from the list by its ID using the subscript operator.
         * \param ID The ID of the object to retrieve.
         * \return A pointer to the object, or `nullptr` if not found.
         */
        OL_Type *operator[](int ID) {
            return getObject(ID);
        }

        /**
         * \brief Retrieves an object from the list by its name using the subscript operator.
         * \param name The name of the object to retrieve.
         * \return A pointer to the object, or `nullptr` if not found.
         */
        OL_Type *operator[](std::string name) {
            return getObject(name);
        }

        /**
         * \brief Retrieves an object from the list by its ID using the function call operator.
         * \param ID The ID of the object to retrieve.
         * \return A reference to the object.
         */
        OL_Type &operator()(int ID) {
            return *getObject(ID);
        }

        /**
         * \brief Retrieves an object from the list by its name using the function call operator.
         * \param name The name of the object to retrieve.
         * \return A reference to the object.
         */
        OL_Type &operator()(std::string name) {
            return *getObject(name);
        }

    };
}

#endif //C_ENGINE_OBJECTLIST_HPP