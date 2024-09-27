cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(C-Engine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/C-Engine/testing)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/C-Engine/include)
include_directories(C:/GLFW/include)
include_directories(C:/VulkanSDK/1.3.290.0/Include)

# Add the executable
add_executable(C-Engine ${CMAKE_SOURCE_DIR}/C-Engine/testing/main.cpp)

# Link GLFW and Vulkan libraries
target_link_libraries(C-Engine C:/GLFW/lib-mingw-w64/libglfw3.a)
target_link_libraries(C-Engine C:/VulkanSDK/1.3.290.0/Lib/vulkan-1.lib)