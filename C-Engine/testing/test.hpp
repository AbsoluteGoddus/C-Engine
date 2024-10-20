//
// Created by absol on 9/24/2024.
//

#ifndef C_ENGINE_TEST_HPP
#define C_ENGINE_TEST_HPP

#include <iostream>
#include <sstream>
#include <bin.hpp>
#include <compress.hpp>
#include <CFG.hpp>
#include <thread>
/*#include <curl_hpp/curl.hpp>*/

std::mutex console_mutex;

void asyncWrite(const std::string& txt) {
    std::thread([](const std::string& message) {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << message << std::endl;
    }, txt).detach();
}

class Test : public engine::Behavior {
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
    int frameCount = 0;

public:
    Test() {
        lastTime = std::chrono::high_resolution_clock::now();
    }

    void update() override {
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;

        if (elapsed.count() >= 1.0) {
            double fps = frameCount / elapsed.count();
            std::ostringstream oss;
            oss << "FPS: " << fps;
            asyncWrite(oss.str());

            frameCount = 0;
            lastTime = currentTime;
        }
    }

    void onEvent(engine::Event e) override {
        if (e.what() == "WINDOW_CLOSE") {
            engine::HANDLE.application_running = false;
        }
    }
};

#endif //C_ENGINE_TEST_HPP
