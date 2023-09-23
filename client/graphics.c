#include "graphics.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * required_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

typedef struct QueueFamilyIndices {
    int32_t graphics_family;
    int32_t present_family;
} QueueFamilyIndices;

VkInstance instance_create() {
    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Hello Triangle";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;

    uint32_t glfw_extension_count = 0;
    const char ** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    const char * required_extensions[glfw_extension_count];
    for (int i = 0; i < glfw_extension_count; i++){
        required_extensions[i] = glfw_extensions[i];
    }

    // apple only
//    required_extensions[glfw_extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
//    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
//    glfw_extension_count += 1;
    //

    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = required_extensions;
    create_info.enabledLayerCount = 0;

    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS){
        fprintf(stderr, "Failed to create instance\n");
        exit(-1);
    }

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    VkExtensionProperties extensions[extension_count];
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);
    printf("Available extensions:\n");
    for (uint32_t i = 0; i < extension_count; i++){
        printf("\t%s\n", extensions[i].extensionName);
    }

    return instance;
}

void instance_destroy(VkInstance instance) {
    vkDestroyInstance(instance, NULL);
}

static QueueFamilyIndices physical_device_find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    QueueFamilyIndices queue_family_indices = (QueueFamilyIndices){
        .graphics_family = -1,
        .present_family = -1,
    };

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);

    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families);

    for (int i = 0; i < queue_family_count; i++){
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_family_indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support){
            queue_family_indices.present_family = i;
        }
    }

    return queue_family_indices;
}

static bool physical_device_has_required_extensions(VkPhysicalDevice physical_device){
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, NULL);

    VkExtensionProperties device_extensions[device_extension_count];
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, device_extensions);

    uint32_t required_extensions_length = sizeof(required_device_extensions) / sizeof(required_device_extensions[0]);
    for (uint32_t i = 0; i < required_extensions_length; i++){
        bool extension_found = false;

        for (uint32_t j = 0; j < device_extension_count; j++){
            if (strcmp(required_device_extensions[i], device_extensions[j].extensionName) == 0){
                extension_found = true;
            }
        }

        if (!extension_found){
            return false;
        }
    }

    return true;
}

static bool physical_device_is_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

    if (physical_device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
        return false;
    }

    QueueFamilyIndices queue_family_indices = physical_device_find_queue_families(physical_device, surface);
    if (queue_family_indices.graphics_family < 0 || queue_family_indices.present_family < 0){
        return false;
    }

    if (!physical_device_has_required_extensions(physical_device)){
        return false;
    }

    return true;
}

VkPhysicalDevice physical_device_pick(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);

    VkPhysicalDevice physical_devices[physical_device_count];
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    for (uint32_t i = 0; i < physical_device_count; i++){
        VkPhysicalDevice physical_device = physical_devices[i];
        if (physical_device_is_suitable(physical_device, surface)){
            return physical_device;
        }
    }

    printf("Failed to find a suitable physical device!\n");
    exit(-1);
}

Device device_create(VkInstance instance, VkSurfaceKHR surface) {
    VkPhysicalDevice physical_device = physical_device_pick(instance, surface);
    QueueFamilyIndices queue_family_indices = physical_device_find_queue_families(physical_device, surface);

    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_infos[2];
    queue_create_infos[0] = (VkDeviceQueueCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family_indices.graphics_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    uint32_t queue_create_count = 1;

    if (queue_family_indices.graphics_family != queue_family_indices.present_family){
        queue_create_infos[1] = (VkDeviceQueueCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_family_indices.present_family,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
        queue_create_count++;
    }

    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo device_create_info = (VkDeviceCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queue_create_infos,
        .queueCreateInfoCount = queue_create_count,
        .pEnabledFeatures = &device_features,
        .ppEnabledExtensionNames = required_device_extensions,
        .enabledExtensionCount = sizeof(required_device_extensions) / sizeof(required_device_extensions[0]),
        .enabledLayerCount = 0,
    };

    VkDevice logical_device;
    if (vkCreateDevice(physical_device, &device_create_info, NULL, &logical_device)){
        printf("Failed to create device!\n");
        exit(-1);
    }

    VkQueue graphics_queue;
    vkGetDeviceQueue(logical_device, queue_family_indices.graphics_family, 0, &graphics_queue);

    VkQueue present_queue;
    vkGetDeviceQueue(logical_device, queue_family_indices.present_family, 0, &present_queue);

    return (Device){
        .physical_device = physical_device,
        .logical_device = logical_device,
        .graphics_queue = graphics_queue,
        .present_queue = present_queue,
    };
}

VkSurfaceKHR surface_create(VkInstance instance, Window window) {
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, window.glfw_window, NULL, &surface);
    return surface;
}

void device_destroy(const Device device) {
    vkDestroyDevice(device.logical_device, NULL);
}

void surface_destroy(const VkInstance instance, const VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, NULL);
}
