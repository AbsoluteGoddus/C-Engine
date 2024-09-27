//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_BEHAVIOR_FNPRE_POSTUPDATE_HPP
#define C_ENGINE_BEHAVIOR_FNPRE_POSTUPDATE_HPP

#include "../engine.hpp"

class exampleBehavior : public engine::Behavior {
public:
    void preUpdate() override {
        // The PreUpdate function is called before the main update loop.
        // This is where you can put logic that needs to run before the update.
        // For example, preparing data or resetting states.
        // Example:
        std::cout << "PreUpdate function called" << std::endl;
        // Output:
        // PreUpdate function called
    }

    void postUpdate() override {
        // The PostUpdate function is called after the main update loop.
        // This is where you can put logic that needs to run after the update.
        // For example, cleaning up or finalizing states.
        // Example:
        std::cout << "PostUpdate function called" << std::endl;
        // Output:
        // PostUpdate function called
    }
};

#endif //C_ENGINE_BEHAVIOR_FNPRE_POSTUPDATE_HPP