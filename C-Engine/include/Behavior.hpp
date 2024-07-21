//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_BEHAVIOR_HPP
#define C_ENGINE_BEHAVIOR_HPP

#include <stdexcept>
#include "Event.hpp"

namespace engine {
    class Behavior {
    public:
        virtual void init() {};
        virtual void preUpdate() {};
        virtual void update() {};
        virtual void postUpdate() {};
        virtual void onEvent(engine::Event &e) {};

        virtual ~Behavior() {};
    };

    class BehaviorPTR {
    private:
        Behavior *_b;

    public:

        template<class BC>
        void setBehavior() {
            _b = new BC;
        }

        void init() {
            _b->init();
        }

        void preUpdate() {
            _b->preUpdate();
        }

        void update() {
            _b->update();
        }

        void postUpdate() {
            _b->postUpdate();
        }

        void onEvent(engine::Event &e) {
            _b->onEvent(e);
        }

        ~BehaviorPTR() {
            engine::Event e("BPTR_MemoryFree", "Deletion of Memory through the Garbage Collector or manual deletion.");
            _b->onEvent(e);
            delete _b;
        }
    };
}

#endif //C_ENGINE_BEHAVIOR_HPP
