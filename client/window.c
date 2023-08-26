#include "window.h"

Window window_create(){
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return (Window) {
        .glfw_window = glfwCreateWindow(
            800,
            600,
            "Vulkan window",
            NULL,
            NULL
        )
    };
}