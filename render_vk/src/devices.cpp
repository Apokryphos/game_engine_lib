#include "common/log.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/queue_family.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include "render_vk/vulkan.hpp"
#include <set>
#include <string>
#include <vector>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static bool check_device_extension_support(
    VkPhysicalDevice device,
    const std::vector<const char*>& device_extensions
) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(
        device_extensions.begin(),
        device_extensions.end()
    );

    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

//  ----------------------------------------------------------------------------
static bool device_is_acceptable(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const std::vector<const char*>& device_extensions
) {
    // VkPhysicalDeviceProperties device_properties;
    // vkGetPhysicalDeviceProperties(device, &device_properties);
    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    QueueFamilyIndices indices = find_queue_families(device, surface);

    const bool extensions_supported = check_device_extension_support(
        device,
        device_extensions
    );

    bool swapchain_adequate = false;
    if (extensions_supported) {
        VulkanSwapchainSupport support = query_swapchain_support(device, surface);
        swapchain_adequate = !support.formats.empty() && !support.present_modes.empty();
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    return (
        indices.is_complete() &&
        extensions_supported &&
        swapchain_adequate &&
        features.samplerAnisotropy
    );
}

//  ----------------------------------------------------------------------------
bool init_device(
    VkInstance instance,
    VkPhysicalDevice& physical_device,
    VkSurfaceKHR surface,
    const std::vector<const char*>& device_extensions
) {
    physical_device = VK_NULL_HANDLE;

    //  Get number of available physical devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        return false;
    }

    //  Get available physical devices
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    //  List available physical devices
    log_debug("%d physical devices available.", device_count);
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        log_debug("\t%s", device_properties.deviceName);
    }

    //  Find first device that meets requirments
    for (const auto& device : devices) {
        if (device_is_acceptable(device, surface, device_extensions)) {
            physical_device = device;
            break;
        }
    }

    //  Check if zero acceptable devices were found
    if (physical_device == VK_NULL_HANDLE) {
        log_error("Failed to find GPU that meets requirements.");
        return false;
    }

    return true;
}

//  ----------------------------------------------------------------------------
bool create_logical_device(
    VkPhysicalDevice physical_device,
    VkDevice& device,
    VkQueue& graphics_queue,
    VkQueue& present_queue,
    VkSurfaceKHR& surface,
    const std::vector<const char*> validation_layers,
    const std::vector<const char*> device_extensions
) {
    QueueFamilyIndices indices = find_queue_families(physical_device, surface);

    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    // VkDeviceQueueCreateInfo queue_create_info{};
    // queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // queue_create_info.queueFamilyIndex = indices.graphics_family.value();
    // queue_create_info.queueCount = 1;

    float queue_priority = 1.0f;
    // queue_create_info.pQueuePriorities = &queue_priority;
    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceVulkan12Features features_12;
    features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_12.descriptorIndexing = VK_TRUE;
    features_12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    VkPhysicalDeviceFeatures2 device_features{};
    device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device_features.features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;

    device_features.features.samplerAnisotropy = VK_TRUE;
    device_features.pNext = &features_12;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = NULL;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.pNext = &device_features;

    if (validation_layers.size() > 0) {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS)  {
        log_error("Failed to create logical device.");
        return false;
    }

    init_vulkan_debug_utils(device);

    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);

    return true;
}
}
