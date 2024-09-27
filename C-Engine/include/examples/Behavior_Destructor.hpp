//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_BEHAVIOR_DESTRUCTOR_HPP
#define C_ENGINE_BEHAVIOR_DESTRUCTOR_HPP

#include "../engine.hpp"

class exampleBehavior : public engine::Behavior {
public:
    ~exampleBehavior() override {
        // The Destructor is called when the behavior is destroyed.
        // This is where you can put cleanup logic.
        // For example, releasing resources or saving states.
        // Example:
        std::cout << "Destructor called" << std::endl;
        // Output:
        // Destructor called
    }
};

#endif //C_ENGINE_BEHAVIOR_DESTRUCTOR_HPP