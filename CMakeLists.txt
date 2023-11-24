cmake_minimum_required(VERSION 3.24)
project(imbored)

set(CMAKE_CXX_STANDARD 17)
set(GLFW C:/Users/Administrator/Desktop/glfw-3.3.8.bin.WIN64)
set(VULKAN C:/VulkanSDK/1.3.268.0)

add_executable(imbored main.cpp vk_core.cpp vk_core.h vk_render.cpp vk_render.h)

include_directories(${GLFW}/include
                    ${VULKAN}/Include)
target_link_libraries(imbored ${GLFW}/lib-mingw-w64/glfw3.dll ${VULKAN}/Lib/vulkan-1.lib)