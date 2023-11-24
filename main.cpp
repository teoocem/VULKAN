

#include<iostream>
#include"vk_render.h"
int main() {

vk_render renderer = {};
renderer.init();

while(!glfwWindowShouldClose(renderer.getWindow())){
    glfwPollEvents();
    renderer.drawFrame();
}
return EXIT_SUCCESS;
}
