#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "window.h"

int main(){
    glfwInit();

    Window window = window_create();

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    printf("Extension count = %d\n", extensionCount);

    while(!glfwWindowShouldClose(window.glfw_window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window.glfw_window);

    glfwTerminate();

    return 0;
}
