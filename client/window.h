#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

typedef struct Window {
    GLFWwindow * glfw_window;
} Window;

Window window_create();
void window_destroy(const Window * window);

#endif
