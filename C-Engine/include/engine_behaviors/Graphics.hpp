//
// Created by absol on 10/19/2024.
//

#ifndef C_ENGINE_GRAPHICS_HPP
#define C_ENGINE_GRAPHICS_HPP

/**
 * @file Graphics.hpp
 * @brief Contains Behaviors provided with C-Engine to communicate with the vulkan renderer.
 */

#include <Behavior.hpp>
#include <GameObject.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine {
    namespace behaviors {
        class graphics_component : public Behavior {
        private:
            engine::GameObject* _obj = nullptr;
        public:
            void onEvent(engine::Event e) override {
                if (e.what() == "GetGameObj") {
                    _obj = *std::any_cast<engine::GameObject*>(e.getData().getObject(0));
                }
            }

            void init() override {
                if (_obj == nullptr) {
                    throw std::runtime_error("GameObject not set.");
                }

                if (!_obj->Behaviors.contains("texture") || !_obj->Behaviors.contains("mesh")) {
                    throw std::runtime_error("GameObject does not contain required behaviors.");
                }
            }
        };

        /**
         * @class texture
         * @brief A behavior that represents a texture.
         * @details This behavior is used to represent a texture in the vulkan renderer.
         */
        class texture : public Behavior {
        private:
            unsigned char* _data;
            int _width, _height, _channels;
            std::string _path = "";

        public:
            void init() override {
                if (_path.empty()) {
                    return;
                } else {
                    loadImage(_path);
                }
            }

            void setLoadPath(const std::string& path) {
                _path = path;
            }

            unsigned char* getImageData() {
                return _data;
            }

            int getImageWidth() {
                return _width;
            }

            int getImageHeight() {
                return _height;
            }

            int getImageChannels() {
                return _channels;
            }

            void loadImage(const std::string& path) {
                _data = stbi_load(path.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
                if (!_data) {
                    throw std::runtime_error("Failed to load image.");
                }
            }
        };
    }
}

#endif //C_ENGINE_GRAPHICS_HPP
