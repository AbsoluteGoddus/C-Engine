//
// Created by absol on 24/06/2024.
//

#ifndef C_ENGINE_ENGINE_HPP
#define C_ENGINE_ENGINE_HPP

#include "Scene.hpp"

namespace engine {
    class ApplicationHandle {
    public:

        bool application_running = false;

        engine::ObjectList<Scene> Scenes;
        int activeSceneID = 0;
        int oldActiveSceneID = 0;

        void init() {
            Scenes[activeSceneID]->init();
        }

        void update() {
            Scenes[activeSceneID]->update();
            if (activeSceneID != oldActiveSceneID) {
                Scenes[oldActiveSceneID]->cleanup();
                oldActiveSceneID = activeSceneID;
                Scenes[activeSceneID]->init();
            }
        }

        void cleanup() {
            Scenes[activeSceneID]->cleanup();
        }
    };

    ApplicationHandle HANDLE;

    void run() {
        HANDLE.init();
        HANDLE.application_running = true;

        while (HANDLE.application_running) {
            HANDLE.update();
        }

        HANDLE.cleanup();
    }

    void debug() {
        try {
            run();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

#endif //C_ENGINE_ENGINE_HPP
