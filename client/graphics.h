#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.h"

VkInstance instance_create();
void instance_destroy(VkInstance instance);

typedef struct Device {
    VkDevice logical_device;
    VkPhysicalDevice physical_device;
    VkQueue graphics_queue;
    VkQueue present_queue;
} Device;

Device device_create(VkInstance instance, VkSurfaceKHR surface);
void device_destroy(Device device);

VkSurfaceKHR surface_create(VkInstance instance, Window window);
void surface_destroy(VkInstance instance, VkSurfaceKHR surface);

VkSwapchainKHR swapchain_create(Window window, Device device, VkSurfaceKHR surface);
void swapchain_destroy(VkSwapchainKHR swapchain, Device device);

#endif
