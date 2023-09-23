#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "graphics.h"
#include "window.h"

int main(){
    glfwInit();

    Window window = window_create();
    VkInstance instance = instance_create();
    VkSurfaceKHR surface = surface_create(instance, window);
    Device device = device_create(instance, surface);

    while(!glfwWindowShouldClose(window.glfw_window)) {
        glfwPollEvents();
    }

    device_destroy(device);
    surface_destroy(instance, surface);
    instance_destroy(instance);
    window_destroy(&window);
    glfwTerminate();

    return 0;
}
