//
// Created by absol on 6/17/2024.
//

#ifndef C_ENGINE_GAMEOBJECT_HPP
#define C_ENGINE_GAMEOBJECT_HPP

#include "ObjectList.hpp"
#include "Behavior.hpp"

namespace engine {
    class GameObject {
    private:
        ObjectList<BehaviorPTR> Behaviors;
    };
}

#endif //C_ENGINE_GAMEOBJECT_HPP
