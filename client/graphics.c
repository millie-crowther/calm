#include "graphics.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_PRESENT_MODES 10

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

    // apple only
//    const char * required_extensions[glfw_extension_count];
//    for (int i = 0; i < glfw_extension_count; i++){
//        required_extensions[i] = glfw_extensions[i];
//    }
//    required_extensions[glfw_extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
//    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
//    glfw_extension_count += 1;
    //

    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;

    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS){
        fprintf(stderr, "Failed to create instance\n");
        exit(-1);
    }

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    VkExtensionProperties * extensions = malloc(sizeof(VkExtensionProperties) * extension_count);
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extensions);
    printf("Available extensions:\n");
    for (uint32_t i = 0; i < extension_count; i++){
        printf("\t%s\n", extensions[i].extensionName);
    }
    free(extensions);

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

    VkQueueFamilyProperties * queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
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

    free(queue_families);
    return queue_family_indices;
}

static bool physical_device_has_required_extensions(VkPhysicalDevice physical_device){
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, NULL);

    VkExtensionProperties * device_extensions = malloc(sizeof(VkExtensionProperties) * device_extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &device_extension_count, device_extensions);

    uint32_t required_extensions_length = sizeof(required_device_extensions) / sizeof(required_device_extensions[0]);
    bool result = true;
    for (uint32_t i = 0; i < required_extensions_length; i++){
        bool extension_found = false;

        for (uint32_t j = 0; j < device_extension_count; j++){
            if (strcmp(required_device_extensions[i], device_extensions[j].extensionName) == 0){
                extension_found = true;
            }
        }

        if (!extension_found){
            result = false;
            break;
        }
    }

    free(device_extensions);
    return result;
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

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
    if (format_count == 0){
        return false;
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
    if (present_mode_count == 0){
        return false;
    }

    return true;
}

VkSurfaceFormatKHR choose_swap_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface){
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);

    VkSurfaceFormatKHR * surface_formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
    VkSurfaceFormatKHR result = surface_formats[0];
    for (uint32_t i = 1; i < format_count; i++){
        if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            result = surface_formats[i];
            break;
        }
    }

    free(surface_formats);
    return result;
}

VkPresentModeKHR choose_present_mode(VkPhysicalDevice physical_device, VkSurfaceKHR surface){
    uint32_t present_mode_count;
    VkPresentModeKHR present_modes[MAXIMUM_PRESENT_MODES];
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);

    for (uint32_t i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(Window window, VkSurfaceCapabilitiesKHR capabilities){

    if (capabilities.currentExtent.width != UINT32_MAX){
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window.glfw_window, &width, &height);

    VkExtent2D extent = (VkExtent2D){
        .width = (uint32_t) width,
        .height = (uint32_t) height,
    };

    extent.width = extent.width < capabilities.minImageExtent.width ? capabilities.minImageExtent.width : extent.width;
    extent.height = extent.height < capabilities.minImageExtent.height ? capabilities.minImageExtent.height : extent.height;
    extent.width = extent.width > capabilities.maxImageExtent.width ? capabilities.maxImageExtent.width : extent.width;
    extent.height = extent.height > capabilities.maxImageExtent.height ? capabilities.maxImageExtent.height : extent.height;

    return extent;
}

VkSwapchainKHR swapchain_create(Window window, Device device, VkSurfaceKHR surface){
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(device.physical_device, surface);
    VkPresentModeKHR present_mode = choose_present_mode(device.physical_device, surface);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device, surface, &capabilities);
    VkExtent2D extent = choose_swap_extent(window, capabilities);

    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
    };

    QueueFamilyIndices indices = physical_device_find_queue_families(device.physical_device, surface);
    if (indices.graphics_family != indices.present_family) {
        uint32_t queue_family_indices[] = {indices.graphics_family, indices.present_family};
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = NULL;
    }

    VkSwapchainKHR swapchain;
    VkResult result = vkCreateSwapchainKHR(device.logical_device, &swapchain_create_info, NULL, &swapchain);
    // NOTE: possibly failing due to full screen request?
    //       https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkCreateSwapchainKHR.html
    //       VK_ERROR_INITIALIZATION_FAILED
    if (result != VK_SUCCESS){
        printf("Failed to create swapchain!\n");
        exit(-1);
    }

    return swapchain;
}

VkPhysicalDevice physical_device_pick(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);

    VkPhysicalDevice * physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_device_count);
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    for (uint32_t i = 0; i < physical_device_count; i++){
        VkPhysicalDevice physical_device = physical_devices[i];
        if (physical_device_is_suitable(physical_device, surface)){
            free(physical_devices);
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

void device_destroy(Device device) {
    vkDestroyDevice(device.logical_device, NULL);
}

void surface_destroy(VkInstance instance, VkSurfaceKHR surface) {
    vkDestroySurfaceKHR(instance, surface, NULL);
}

void swapchain_destroy(VkSwapchainKHR swapchain, Device device) {
    vkDestroySwapchainKHR(device.logical_device, swapchain, NULL);
}
