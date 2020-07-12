#include "common/log.hpp"
#include "platform/glfw_init.hpp"
#include "render_vk/debug_utils.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

using namespace common;

namespace render_vk
{
//  ----------------------------------------------------------------------------
static bool check_validation_layers_support(
    const std::vector<const char*>& validation_layers
) {
    //  Get number of available validation layers
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    //  Get available validation layers
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    //  List available validation layers
    log_debug("%d Vulkan layers available.", layer_count);
    for (const auto& layer_properties : available_layers) {
        log_debug("\t%s", layer_properties.layerName);
    }

    //  Verify that all requested validation layers exist
    for (const char* layer_name : validation_layers) {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

//  ----------------------------------------------------------------------------
bool create_instance(
    VkInstance& instance,
    const std::vector<const char*>& validation_layers
) {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "render_vk";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    uint32_t count;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    if (glfw_extensions == NULL) {
        log_error("Vulkan is not supported.");
        return false;
    }

    //  Copy required extensions
    std::vector<const char*> extensions(count);
    for (uint32_t n = 0; n < count; ++n) {
        extensions[n] = glfw_extensions[n];
    }

    //  Check if debug utils extension is supported
    if (check_debug_utils_support()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = 0;

    //  Validation layers
    #ifdef DEBUG
    const bool enable_validation_layers = true;
    #else
    const bool enable_validation_layers = false;
    #endif

    if (enable_validation_layers) {
        log_debug("Initializing Vulkan validation layers.");
        if (!check_validation_layers_support(validation_layers)) {
            log_error("Vulkan validation layers unavailable.");
            return false;
        }

        create_info.enabledLayerCount = validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    //  Vulkan instance
    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        log_error("Failed to create Vulkan instance.");
        return false;
    }

    log_debug("Created Vulkan instance.");

    return true;
}
}
