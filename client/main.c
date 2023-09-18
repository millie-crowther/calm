#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "graphics.h"
#include "window.h"

int main(){
    glfwInit();

    Window window = window_create();
    VkInstance instance = instance_create();
    VkPhysicalDevice physical_device = physical_device_pick(instance);

    while(!glfwWindowShouldClose(window.glfw_window)) {
        glfwPollEvents();
    }

    instance_destroy(instance);
    window_destroy(&window);
    glfwTerminate();

    return 0;
}
