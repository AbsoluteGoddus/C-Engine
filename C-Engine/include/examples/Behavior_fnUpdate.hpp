//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_BEHAVIOR_FNUPDATE_HPP
#define C_ENGINE_BEHAVIOR_FNUPDATE_HPP

#include "../engine.hpp"

class exampleBehavior : public engine::Behavior {
public:
    void update() override {
        // The Update function is called every tick.
        // This is where you can put your game logic.
        // For example, moving a sprite or checking for collisions.
        // Example:
        std::cout << "Update function called" << std::endl;
        // Output:
        // Update function called
    }
};

#endif //C_ENGINE_BEHAVIOR_FNUPDATE_HPP
