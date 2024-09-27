//
// Created by absol on 9/24/2024.
//

#define C_ENGINE_USE_VULKAN
#define C_ENGINE_USE_VKLOG
#define C_ENGINE_VKLOG_FILTER_ALL

#include <engine.hpp>
#include "test.hpp"

int main() {
    engine::Scene scene;
    engine::GameObject go;
    engine::Behavior behavior_test;
    engine::BehaviorPTR bptr;
    bptr.setBehavior<Test>();

    go.Behaviors.addObject(bptr);
    scene.Objects.addObject(go);

    engine::HANDLE.Scenes.addObject(scene);

    engine::debug();

    return 0;
}