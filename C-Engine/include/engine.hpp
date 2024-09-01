//
// Created by absol on 24/06/2024.
//

#ifndef C_ENGINE_ENGINE_HPP
#define C_ENGINE_ENGINE_HPP

#include "Scene.hpp"
#include <functional>
#include <vector>

namespace engine {
    class ApplicationHandle {
    public:

        std::vector<std::function<void()>> extInit;
        std::vector<std::function<void()>> extUpdate;
        std::vector<std::function<void()>> extUpdateFix;
        std::vector<std::function<void()>> extTerminate;

        bool application_running = false;

        engine::ObjectList<Scene> Scenes;
        int activeSceneID = 0;
        int oldActiveSceneID = 0;

        void init() {
            for (auto ext : extInit) {
                ext();
            }
            Scenes[activeSceneID]->init();
        }

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

        void cleanup() {
            Scenes[activeSceneID]->cleanup();
            for (auto ext : extTerminate) {
                ext();
            }
        }
    };

    /**
     * Used to control the application
     */
    ApplicationHandle HANDLE;

    /**
     * Runs the application
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
     * Runs the application with additional error handling
     */
    void debug() {
        try {
            run();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

#endif //C_ENGINE_ENGINE_HPP
