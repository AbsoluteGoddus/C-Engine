// Node.hpp
// Created by absol on 7/3/2024.

#ifndef NODE_HPP
#define NODE_HPP

#include <any>
#include <string>
#include <unordered_map>
#include <utility>
#include <iterator>
#include <stdexcept>

/**
 * \typedef nodeValue
 * \brief A pair of a string and an any type.
 */
using nodeValue = std::pair<std::string, std::any>;

/**
 * \file Node.hpp
 * \brief Defines the Node class used for creating a hierarchical structure of nodes.
 */

/**
 * \class Node
 * \brief Represents a node that can contain sub-nodes and values.
 *
 * The Node class is used to create a hierarchical structure of nodes, where each node can contain sub-nodes and values.
 * Each node has a name and can have a parent node.
 */
class Node {
private:
    using nodeType = std::unordered_map<std::string, Node>;
    using valueType = std::unordered_map<std::string, std::any>;

    std::string _name;  /**< The name of the node. */
    std::unordered_map<std::string, Node> _subNodes; /**< Sub-nodes stored in an unordered map. */
    std::unordered_map<std::string, std::any> _values; /**< Values stored in an unordered map. */
    Node *_parent = nullptr; /**< Pointer to the parent node. */

public:

    /**
     * \brief Initializes the Node with a name.
     * \param name The name to be used for the node.
     */
    Node(const std::string& name = "root") : _name(name) {}

    /**
     * \brief Returns the size of the node.
     *
     * Recursively calculates and returns the size of the current node, including all sub-nodes.
     *
     * \return The total size of the node and its sub-nodes.
     */
    size_t size() const {
        size_t base = _values.size() + _subNodes.size();
        for (const auto& e : _subNodes) {
            base += e.second.size();
        }
        return base;
    }

    /**
     * \brief Sets the parent of the Node.
     * \param _p The node to be set as the parent.
     */
    void setParent(Node *_p) {
        _parent = _p;
    }

    /**
     * \brief Gets the parent of the Node.
     * \return A pointer to the parent node.
     */
    Node* getParent() const {
        return _parent;
    }

    /**
     * \brief Gets the name of the Node.
     * \return The name of the Node.
     */
    std::string getName() const {
        return _name;
    }

    /**
     * \brief Sets the name of the Node.
     * \param name The new name for the node.
     */
    void setName(const std::string& name) {
        _name = name;
    }

    /**
     * \brief Adds a sub-node to this node.
     * \param n The node to be added as a sub-node.
     */
    void pushNode(Node n) {
        _subNodes.insert({n.getName(), std::move(n)});
    }

    /**
     * \brief Adds a value to this node.
     * \param name The name of the new value.
     * \param value The value to be added.
     */
    void pushValue(const std::string& name, std::any value) {
        _values.insert({name, std::move(value)});
    }

    /**
     * \brief Checks whether a certain value exists in the node.
     * \param key The name of the value to check.
     * \return `true` if the value exists, otherwise `false`.
     */
    bool contains(const std::string& key) {
        auto tmp = _values.find(key);
        return (tmp != _values.end());
    }

    /**
     * \brief Inserts a sub-node into the Node using the insertion operator.
     * \param n The Node to be inserted.
     */
    void operator<<(Node n) {
        auto nodeName = n.getName();
        n.setParent(this);
        _subNodes.insert({nodeName, std::move(n)});
    }

    /**
     * \brief Inserts a value into the Node using the insertion operator.
     * \param value The value to be inserted.
     */
    void operator<(nodeValue value) {
        _values.insert({std::move(value.first), std::move(value.second)});
    }

    /**
     * \brief Gets a value reference from the Node.
     * \param key The name of the value to be returned.
     * \return A reference to the value.
     * \throws std::out_of_range if the key is not found.
     */
    std::any& operator()(const std::string& key) {
        auto it = _values.find(key);
        if (it == _values.end()) {
            throw std::out_of_range("Key not found in values: " + key);
        }
        return it->second;
    }

    /**
     * \brief Gets a sub-node reference from the Node.
     * \param key The name of the sub-node.
     * \return A reference to the sub-node.
     */
    Node& operator[](const std::string& key) {
        auto it = _subNodes.find(key);
        if (it == _subNodes.end()) {
            Node tmp(key);
            tmp.setParent(this);
            it = _subNodes.emplace(key, std::move(tmp)).first;
        }
        return it->second;
    }

    /**
     * \brief Gets the sub-nodes map.
     * \return A const reference to the sub-nodes map.
     */
    const std::unordered_map<std::string, Node>& getSubNodes() const {
        return _subNodes;
    }

    /**
     * \brief Gets the values map.
     * \return A const reference to the values map.
     */
    const std::unordered_map<std::string, std::any>& getValues() const {
        return _values;
    }
};

#endif // NODE_HPP