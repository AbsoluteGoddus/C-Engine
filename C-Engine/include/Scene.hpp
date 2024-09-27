//
// Created by absol on 19/06/2024.
//

#ifndef C_ENGINE_SCENE_HPP
#define C_ENGINE_SCENE_HPP

#include "GameObject.hpp"
#include <iostream>

/**
 * \file Scene.hpp
 * \brief Defines the Scene class used for managing a collection of GameObjects.
 */

namespace engine {
    /**
     * \class Scene
     * \brief Manages a collection of GameObjects and provides methods to initialize, update, and clean up the scene.
     *
     * The Scene class contains a list of GameObjects and provides methods to initialize, update, and clean up these objects.
     * It also handles events related to the cleanup and destruction of the scene.
     */
    class Scene {
    public:
        ObjectList<GameObject> Objects; /**< List of GameObjects in the scene. */

        /**
         * \brief Initializes all GameObjects in the scene.
         */
        void init() {
            for (auto &o : Objects) {
                o.init();
            }
        }

        /**
         * \brief Updates all GameObjects in the scene.
         *
         * This method performs a pre-update, update, and post-update step for each GameObject in the scene.
         */
        void update() {
            for (auto &o  : Objects) {
                o.preUpdate();
            }

            for (auto &o : Objects) {
                o.update();
            }

            for (auto &o : Objects) {
                o.postUpdate();
            }
        }

        /**
         * \brief Cleans up the scene by sending a cleanup event to all GameObjects.
         */
        void cleanup() {
            for (auto &o : Objects) {
                engine::Event e("SCENE_Cleanup", "Cleanup due to call of 'cleanup()' in Scene.hpp:36");
                o.onEvent(e);
            }
        }

        /**
         * \brief Destructor for the Scene class.
         *
         * Sends a destroy event to all GameObjects when the scene is destroyed.
         */
        ~Scene() {
            for (auto o : Objects) {
                engine::Event e("SCENE_Destroy", "Destroy due to deletion of Scene in Scene.hpp:43");
                o.onEvent(e);
            }
        }
    };
}

#endif //C_ENGINE_SCENE_HPP