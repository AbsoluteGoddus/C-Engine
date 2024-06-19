//
// Created by absol on 19/06/2024.
//

#ifndef C_ENGINE_SCENE_HPP
#define C_ENGINE_SCENE_HPP

#include "GameObject.hpp"

namespace engine {
    class Scene {
    public:
        ObjectList<GameObject> Objects;

        void init() {
            for (auto o : Objects) {
                o.init();
            }
        }

        void update() {
            for (auto o : Objects) {
                o.preUpdate();
            }

            for (auto o : Objects) {
                o.update();
            }

            for (auto o : Objects) {
                o.postUpdate();
            }
        }

        void cleanup() {
            for (auto o : Objects) {
                engine::Event e("Cleanup", "Cleanup due to call of 'cleanup()' in Scene.hpp:35");
                o.onEvent(e);
            }
        }

        ~Scene() {
            for (auto o : Objects) {
                engine::Event e("Destroy", "Destroy due to deletion of Scene in Scene.hpp:41");
                o.onEvent(e);
            }
        }
    };
}

#endif //C_ENGINE_SCENE_HPP
