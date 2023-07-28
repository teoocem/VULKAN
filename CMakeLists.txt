cmake_minimum_required(VERSION 3.24)
project(VULKAN)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR C:/Users/Administrator/Desktop/glfw-3.3.8.bin.WIN64)
include_directories(${GLFW_DIR}/include/GLFW)

set(VULKAN_SDK C:/VulkanSDK/1.3.250.1)
include_directories(${VULKAN_SDK}/Include)

set(SHADER_FILES C:/Users/Administrator/CLionProjects/untitled/shaders)


add_executable(VULKAN main.cpp Renderer.cpp Renderer.h Pipeline.cpp Pipeline.h)
target_link_libraries(VULKAN ${GLFW_DIR}/lib-mingw-w64/glfw3.dll)
target_link_libraries(VULKAN ${VULKAN_SDK}/Lib/vulkan-1.lib)
