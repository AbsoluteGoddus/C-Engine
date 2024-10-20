//
// Created by absol on 24/06/2024.
//

#ifndef C_ENGINE_ENGINE_HPP
#define C_ENGINE_ENGINE_HPP

#include <vector>
#include "Scene.hpp"
#include <functional>
#ifdef C_ENGINE_USE_VULKAN
#include "vulkan.hpp"
#endif

/**
 * \file engine.hpp
 * \brief Defines the ApplicationHandle class used for managing the application lifecycle.
 */

namespace engine {
    /**
     * \class ApplicationHandle
     * \brief Manages the application lifecycle, including initialization, updating, and cleanup.
     *
     * The ApplicationHandle class is responsible for managing the application's scenes and external functions.
     * It provides methods to initialize, update, and clean up the application.
     */
    class ApplicationHandle {
    public:
        /**
         * \brief Behavior to handle GLFW events if \c C_ENGINE_USE_VULKAN is defined.
         *
         * If \c C_ENGINE_USE_VULKAN is not defined, this will be ignored.
         * If not set, the default behavior will be to close the application when the window is closed.
         * Other events will be ignored.
         */
        BehaviorPTR glfwEvent_bptr;

        std::vector<std::function<void()>> extInit; /**< External initialization functions. */
        std::vector<std::function<void()>> extUpdate; /**< External update functions. */
        std::vector<std::function<void()>> extUpdateFix; /**< External fixed update functions. */
        std::vector<std::function<void()>> extTerminate; /**< External termination functions. */

        bool application_running = false; /**< Indicates whether the application is running. */

        engine::ObjectList<Scene> Scenes; /**< List of scenes in the application. */
        int activeSceneID = 0; /**< ID of the currently active scene. */
        int oldActiveSceneID = 0; /**< ID of the previously active scene. */

        /**
         * \brief Initializes the application and its scenes.
         */
        void init() {
            for (auto ext : extInit) {
                ext();
            }
            Scenes[activeSceneID]->init();
        }

        /**
         * \brief Updates the application and its scenes.
         *
         * This method updates the active scene and handles scene transitions.
         */
        void update() {
            for (auto ext : extUpdate) {
                ext();
            }
            Scenes[activeSceneID]->update();
            if (activeSceneID != oldActiveSceneID) {
                Scenes[oldActiveSceneID]->cleanup();
                oldActiveSceneID = activeSceneID;
                Scenes[activeSceneID]->init();
            }
            for (auto ext : extUpdateFix) {
                ext();
            }
        }

        /**
         * \brief Cleans up the application and its scenes.
         */
        void cleanup() {
            Scenes[activeSceneID]->cleanup();
            for (auto ext : extTerminate) {
                ext();
            }
        }
    };

    /**
     * \brief Used to control the application.
     */
    ApplicationHandle HANDLE;

#ifndef C_ENGINE_USE_VULKAN
    /**
     * \brief Runs the application.
     *
     * This method initializes the application, enters the main loop, and performs cleanup.
     */
    void run() {
        HANDLE.init();
        HANDLE.application_running = true;

        while (HANDLE.application_running) {
            HANDLE.update();
        }

        HANDLE.cleanup();
    }

    /**
     * \brief Runs the application with additional error handling.
     */
    void debug() {
        try {
            run();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
    }
#else
    /**
     * \brief Runs the application with Vulkan support.
     *
     * This method initializes Vulkan, enters the main loop, and performs cleanup.
     */
    void run() {
        VulkanBridge app;
        app.initWindow();
        app.initVulkan();
        HANDLE.init();
        HANDLE.application_running = true;

        while (HANDLE.application_running) {
            glfwPollEvents();
            {
                if (HANDLE.glfwEvent_bptr.isSet()) {
                    if (glfwWindowShouldClose(app.getWindow())) {
                        Event e("WINDOW_CLOSE", "GLFW Issued a Window Close Event");
                        HANDLE.glfwEvent_bptr.onEvent(e);
                    }
                } else {
                    if (glfwWindowShouldClose(app.getWindow())) {
                        HANDLE.application_running = false;
                    }
                }
            }
            app.draw();
            HANDLE.update();
        }

#ifdef C_ENGINE_USE_VKLOG
        {
            auto log = app.getVulkanLog();
            std::ofstream file("vklog.log");
            file.clear();
            file << log;
            file.close();
        }
#endif

        app.cleanup();
        HANDLE.cleanup();
    }

    /**
     * \brief Runs the application with additional error handling and Vulkan support.
     */
    void debug() {
        try {
            run();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
    }

#endif
}

#endif //C_ENGINE_ENGINE_HPP