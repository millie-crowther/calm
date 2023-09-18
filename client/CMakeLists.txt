cmake_minimum_required(VERSION 3.21)
project(calm C)
set(CMAKE_C_STANDARD 23)
#set(Vulkan_INCLUDE_DIR /Users/personal/VulkanSDK/1.3.261.0/macOS/include)
#set(Vulkan_LIBRARY /Users/personal/VulkanSDK/1.3.261.0/macOS/lib/libvulkan.dylib)
set(Vulkan_INCLUDE_DIR /home/millie/VulkanSDK/1.3.261.1/x86_64/include)
set(Vulkan_LIBRARY /home/millie/VulkanSDK/1.3.261.1/x86_64/lib/libvulkan.so)

add_subdirectory(lib/glfw)
find_package(Vulkan REQUIRED)
add_executable(calm main.c window.c
        graphics.c
        graphics.h)
target_link_libraries(calm glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)