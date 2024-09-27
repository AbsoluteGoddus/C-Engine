//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_BEHAVIOR_FNINIT_HPP
#define C_ENGINE_BEHAVIOR_FNINIT_HPP

#include "../engine.hpp"

class exampleBehavior : public engine::Behavior {
public:
    void init() override {
        // The Init function is called once when the behavior is first created.
        // This is where you can put initialization logic.
        // For example, setting initial values or loading resources.
        // Example:
        std::cout << "Init function called" << std::endl;
        // Output:
        // Init function called
    }
};

#endif //C_ENGINE_BEHAVIOR_FNINIT_HPP