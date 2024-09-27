//
// Created by absol on 9/27/2024.
//

#ifndef C_ENGINE_BEHAVIOR_FNONEVENT_HPP
#define C_ENGINE_BEHAVIOR_FNONEVENT_HPP

#include "../engine.hpp"

#define C_ENGINE_USE_VULKAN

class exampleBehavior : public engine::Behavior {
public:
    void onEvent(engine::Event e) override {
        // The onEvent function is called when an event occurs.
        // This is where you can handle events.
        // For example, processing input or reacting to game events.
        // If you use C_ENGINE_USE_VULKAN, you can also handle Vulkan / GLFW events, such as WINDOW_CLOSE & more.
        // Example:
        if (e.what() == "WINDOW_CLOSE") { // Check if the event is a window close event.
            engine::HANDLE.application_running = false; // Set the application_running flag to false, to stop the application.
        }
        // Output (as of version 0.0.4):
        // Opens a window, displaying a colored triangle & closes the window if the 'X' button, in the top right of the window, is clicked.
    }
};

#endif //C_ENGINE_BEHAVIOR_FNONEVENT_HPP