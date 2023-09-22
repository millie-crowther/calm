#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct QueueFamilyIndices {
    int32_t graphics_family;
} QueueFamilyIndices;

typedef struct Device {
    VkDevice logical_device;
    VkPhysicalDevice physical_device;
    QueueFamilyIndices queue_family_indices;
} Device;

VkInstance instance_create();
void instance_destroy(VkInstance instance);

Device device_create(VkInstance instance);

#endif
