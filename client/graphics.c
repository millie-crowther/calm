#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>

VkInstance create_instance() {
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

    uint32_t extension_count = 0;
    const char ** glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    const char * required_extensions[extension_count + 1];
    for (int i = 0; i < extension_count; i++){
        required_extensions[i] = glfw_extensions[i];
    }

    // apple only
    required_extensions[extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    extension_count += 1;
    //

    create_info.enabledExtensionCount = extension_count;
    create_info.ppEnabledExtensionNames = required_extensions;
    create_info.enabledLayerCount = 0;

    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    printf("result = %d, x = %d\n", result, VK_ERROR_INCOMPATIBLE_DRIVER);
    if (result != VK_SUCCESS){
        fprintf(stderr, "Failed to create instance\n");
        exit(-1);
    }

    return instance;
}
