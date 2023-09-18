#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkInstance instance_create();
void instance_destroy(VkInstance instance);

#endif
