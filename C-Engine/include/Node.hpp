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

using nodeValue = std::pair<std::string, std::any>;

class Node {
private:
    using nodeType = std::unordered_map<std::string, Node>;
    using valueType = std::unordered_map<std::string, std::any>;

    std::string _name;  // Name of the node

    // Sub-nodes and values stored in unordered maps
    std::unordered_map<std::string, Node> _subNodes;
    std::unordered_map<std::string, std::any> _values;

    Node *_parent = nullptr;

public:

    /**
     * Initializes the Node with a name
     * @param name The name to be used
     */
    Node(const std::string& name = "root") : _name(name) {}

    /**
     * @brief Returns the size of the node
     *
     * Recursively adds and returns the size of the current node, as well as every sub node
     *
     * @return
     */
    size_t size() const {
        size_t base = _values.size() + _subNodes.size();
        for (const auto& e : _subNodes) {
            base += e.second.size();
        }
        return base;
    }

    /**
     * Sets the parent of the Node
     * @param _p The node to be inserted as the parent
     */
    void setParent(Node *_p) {
        _parent = _p;
    }

    /**
     * Gets the parent of the Node
     * @return
     */
    Node* getParent() const {
        return _parent;
    }

    /**
     * Gets the name of the Node
     * @return The name of the Node
     */
    std::string getName() const {
        return _name;
    }

    /**
     * Sets the name of the Node
     * @param name The new name
     */
    void setName(const std::string& name) {
        _name = name;
    }

    /**
     * @brief Adds a Node
     * Adds a Node {n} to this node
     * @param n The node to be added
     */
    void pushNode(Node n) {
        _subNodes.insert({n.getName(), std::move(n)});
    }

    /**
     * @brief Adds a Value
     * Adds a Value {value} to this node
     * @param name The name of the new value
     * @param value The node to be added
     */
    void pushValue(const std::string& name, std::any value) {
        _values.insert({name, std::move(value)});
    }

    /**
     * Checks whether or not a certain value exists in a node
     * @param key
     * @return
     */
    bool contains(const std::string& key) {
        return _values.contains(key);
    }

    /**
     * Inserts a subNode into the Node
     * @param n The Node to be Inserted
     */
    void operator<<(Node n) {
        auto nodeName = n.getName();
        n.setParent(this);
        _subNodes.insert({nodeName, std::move(n)});
    }

    /**
     * Inserts a Value into the Node
     * @param value The Value to be inserted
     */
    void operator<(nodeValue value) {
        _values.insert({std::move(value.first), std::move(value.second)});
    }

    /**
     * Gets a value reference from the Node
     * @param key The name of the value to be returned
     * @return
     */
    std::any& operator()(const std::string& key) {
        auto it = _values.find(key);
        if (it == _values.end()) {
            throw std::out_of_range("Key not found in values: " + key);
        }
        return it->second;
    }

    /**
     * Gets a subNode reference from the Node
     * @param key  The name of the subNode
     * @return
     */
    Node& operator[](const std::string& key) {
        // Try to find the key in the _subNodes map
        auto it = _subNodes.find(key);

        // If the key does not exist, create a new Node
        if (it == _subNodes.end()) {
            Node tmp(key);
            tmp.setParent(this);
            it = _subNodes.emplace(key, std::move(tmp)).first;
        }

        // Return the Node found or created
        return it->second;
    }

    /**
     * Gets the subNodes map
     * @return The subNodes map
     */
    const std::unordered_map<std::string, Node>& getSubNodes() const {
        return _subNodes;
    }

    /**
     * Gets the values map
     * @return The values map
     */
    const std::unordered_map<std::string, std::any>& getValues() const {
        return _values;
    }
};

#endif //NODE_HPP
