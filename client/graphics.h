#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct Device {
    VkDevice logical_device;
    VkPhysicalDevice physical_device;
    VkQueue graphics_queue;
} Device;

VkInstance instance_create();
void instance_destroy(VkInstance instance);

Device device_create(VkInstance instance);

#endif
