//
// Created by absol on 19/06/2024.
//

#include "../../lib/GameObject.hpp"
#include <iostream>

class debug : public engine::Behavior { // Debug Behavior
    void init() override { // Called when the object is instantiated
        std::cout << "Init, Called" << std::endl; // Writes 'Init, Called' to the console
    }

    void update() override { // Called once every calculation tick
        std::cout << "Update, Called" << std::endl; // Writes 'Update, Called' to the console
    }
};

int main() {
    engine::BehaviorPTR b; // Make a BehaviorPTR to contain the Behavior
    b.setBehavior<debug>(); // Set the container to include our debug Behavior

    engine::GameObject o; // Make a GameObject to contain our BehaviorPTR
    o.Behaviors.addObject(b, 1); // Set The Behavior

    o.init(); // Calls init() on every behavior of the GameObject
    o.update(); // Calls update() on every behavior of the GameObject

    return 0; // Return 0 for EXIT_SUCCESS or in other words a successful execution of the program
}