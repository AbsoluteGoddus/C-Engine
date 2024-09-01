//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_GAMEOBJECT_HPP
#define C_ENGINE_GAMEOBJECT_HPP

#include "ObjectList.hpp"
#include "Behavior.hpp"

namespace engine {
    /**
     * Class for containing and executing Behaviors
     */
    class GameObject {
    private:
    public:

        ObjectList<BehaviorPTR> Behaviors;

        void init() {
            for (auto &b : Behaviors) {
                b.init();
            }
        }

        void preUpdate() {
            for (auto &b : Behaviors) {
                b.preUpdate();
            }
        }

        void update() {
            for (auto &b : Behaviors) {
                b.update();
            }
        }

        void postUpdate() {
            for (auto &b : Behaviors) {
                b.postUpdate();
            }
        }

        void onEvent(engine::Event &e) {
            for (auto &b : Behaviors) {
                b.onEvent(e);
            }
        }
    };
}

#endif //C_ENGINE_GAMEOBJECT_HPP
