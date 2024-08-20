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

    // Constructor to initialize node name
    Node(const std::string& name = "root") : _name(name) {}

    size_t size() const {
        size_t base = _values.size() + _subNodes.size();
        for (const auto& e : _subNodes) {
            base += e.second.size();
        }
        return base;
    }

    void setParent(Node *_p) {
        _parent = _p;
    }

    Node* getParent() const {
        return _parent;
    }

    // Get and set the name of the node
    std::string getName() const {
        return _name;
    }

    void setName(const std::string& name) {
        _name = name;
    }

    // Add a sub-node
    void pushNode(Node n) {
        _subNodes.insert({n.getName(), std::move(n)});
    }

    // Add a value
    void pushValue(const std::string& name, std::any value) {
        _values.insert({name, std::move(value)});
    }

    // Checks if the node contains a value
    bool contains(const std::string& key) {
        return _values.contains(key);
    }

    // Overloaded insertion operators for convenience
    void operator<<(Node n) {
        auto nodeName = n.getName();
        n.setParent(this);
        _subNodes.insert({nodeName, std::move(n)});
    }

    void operator<(nodeValue value) {
        _values.insert({std::move(value.first), std::move(value.second)});
    }

    // Access values and sub-nodes
    std::any& operator()(const std::string& key) {
        auto it = _values.find(key);
        if (it == _values.end()) {
            throw std::out_of_range("Key not found in values: " + key);
        }
        return it->second;
    }

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

    // Getters for private members
    const std::unordered_map<std::string, Node>& getSubNodes() const {
        return _subNodes;
    }

    const std::unordered_map<std::string, std::any>& getValues() const {
        return _values;
    }
};

#endif //NODE_HPP
