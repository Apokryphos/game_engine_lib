#include "render_vk/debug_gui/vulkan_debug_panel.hpp"
#include "render_vk/vulkan.hpp"
#include "imgui.h"
#include <string>

namespace render_vk
{
struct VulkanQueueFamilyInfo
{
    VkQueueFamilyProperties properties;
};

struct VulkanPhysicalDeviceInfo
{
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    std::vector<VulkanQueueFamilyInfo> queue_families;

    //  Device extensions
    std::vector<VkExtensionProperties> extensions;
};

struct VulkanInfo
{
    std::vector<VulkanPhysicalDeviceInfo> devices;

    //  Interface extensions
    std::vector<VkExtensionProperties> extensions;
};

//  ----------------------------------------------------------------------------
static void init_vulkan_device_info(
    VkInstance instance,
    VulkanPhysicalDeviceInfo& info
) {
    const VkPhysicalDevice device = info.device;

    //  Get device extension count
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    //  Get device extensions
    info.extensions.resize(extension_count);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extension_count,
        info.extensions.data()
    );

    //  Get physical device features
    vkGetPhysicalDeviceFeatures(info.device, &info.features);

    //  Query device queue family count
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    //  Query properties for each queue family
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    //  Copy queue family properties to info struct
    for (const auto& family : queue_families) {
        VulkanQueueFamilyInfo family_info{};
        family_info.properties = family;
        info.queue_families.push_back(family_info);
    }
}

//  ----------------------------------------------------------------------------
static void init_vulkan_info(VkInstance instance, VulkanInfo& info) {
    //  Get instance extension count
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        nullptr
    );

    //  Get instance extensions
    info.extensions.resize(extension_count);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        info.extensions.data()
    );

    //  Get available physical device count
    uint32_t device_count;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count == 0) {
        return;
    }

    //  Get available physical devices
    std::vector<VkPhysicalDevice> devices;
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    //  Build device infos
    info.devices.reserve(info.devices.size());
    for (const VkPhysicalDevice& device : devices) {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);

        VulkanPhysicalDeviceInfo device_info{};
        device_info.device = device;
        device_info.properties = device_properties;

        init_vulkan_device_info(instance, device_info);

        info.devices.push_back(device_info);
    }
}

//  ----------------------------------------------------------------------------
static void device_listbox(VulkanInfo& info, int& index) {
    auto item_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& devices = *static_cast<std::vector<VulkanPhysicalDeviceInfo>*>(vector);

        if (index < 0 || index >= static_cast<int>(devices.size())) {
            return false;
        }

        *out_text = devices.at(index).properties.deviceName;
        return true;
    };

    //  Listbox
    ImGui::PushItemWidth(-1);
    ImGui::Text("Physical Devices");
    ImGui::ListBox(
        "Physical Devices",
        &index,
        item_getter,
        static_cast<void*>(&const_cast<std::vector<VulkanPhysicalDeviceInfo>&>(info.devices)),
        info.devices.size()
    );
    ImGui::PopItemWidth();
}

//  ----------------------------------------------------------------------------
static bool extensions_listbox(
    const char* label,
    const std::vector<VkExtensionProperties>& extensions,
    int& index
) {
    auto item_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& extensions = *static_cast<std::vector<VkExtensionProperties>*>(vector);

        if (index < 0 || index >= static_cast<int>(extensions.size())) {
            return false;
        }

        const auto& ext = extensions.at(index);

        static std::string ext_name;
        ext_name =
            std::string(ext.extensionName) +
            " (" +
            std::to_string(ext.specVersion) +
            ")";

        *out_text = ext_name.c_str();
        return true;
    };

    //  Listbox
    ImGui::PushItemWidth(-1);
    ImGui::Text(label);
    bool result = ImGui::ListBox(
        label,
        &index,
        item_getter,
        static_cast<void*>(&const_cast<std::vector<VkExtensionProperties>&>(extensions)),
        extensions.size()
    );
    ImGui::PopItemWidth();

    return result;
}


//  ----------------------------------------------------------------------------
static bool queue_families_listbox(const VulkanPhysicalDeviceInfo& info, int& index) {
    auto item_getter = [](
        void* vector,
        int index,
        const char** out_text
    ) {
        auto& families = *static_cast<std::vector<VulkanQueueFamilyInfo>*>(vector);

        if (index < 0 || index >= static_cast<int>(families.size())) {
            return false;
        }

        static std::string family_name;
        family_name = std::to_string(index);

        *out_text = family_name.c_str();
        return true;
    };

    //  Listbox
    ImGui::PushItemWidth(-1);
    ImGui::Text("Queue Families");
    bool result = ImGui::ListBox(
        "Queue Families",
        &index,
        item_getter,
        static_cast<void*>(&const_cast<std::vector<VulkanQueueFamilyInfo>&>(info.queue_families)),
        info.queue_families.size()
    );
    ImGui::PopItemWidth();

    return result;
}

//  ----------------------------------------------------------------------------
void queue_family_panel(const VulkanQueueFamilyInfo& info) {
    if (info.properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
        ImGui::Text("Compute");
    }

    if (info.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        ImGui::Text("Graphics");
    }

    if (info.properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
        ImGui::Text("Transfer");
    }

    if (info.properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
        ImGui::Text("Sparse Memory Management");
    }

    if (info.properties.queueFlags & VK_QUEUE_PROTECTED_BIT) {
        ImGui::Text("Protected Memory");
    }
}

//  ----------------------------------------------------------------------------
void device_features_panel(const VulkanPhysicalDeviceInfo& device_info) {
    //  Device features
    ImGui::Text("Device Supported Features");
    bool feature = device_info.features.shaderSampledImageArrayDynamicIndexing;
    ImGui::Checkbox("shaderSampledImageArrayDynamicIndexing", &feature);
}

//  ----------------------------------------------------------------------------
void vulkan_debug_panel(VkInstance instance) {
    static VulkanInfo info{};

    //  Initialize only once
    static bool initialized = false;
    if (!initialized) {
        init_vulkan_info(instance, info);
        initialized = true;
    }

    if (info.devices.empty()) {
        ImGui::Text("Failed to find Vulkan physical devices found.");
        return;
    }

    //  Instance extensions listbox
    static int instance_ext_index = 0;
    extensions_listbox("Instance Extensions", info.extensions, instance_ext_index);

    ImGui::Separator();

    //  Physical devices listbox
    static int device_index = 0;
    device_listbox(info, device_index);

    //  Select device info using listbox index
    const VulkanPhysicalDeviceInfo& device_info = info.devices.at(device_index);

    //  Vulkan API version
    ImGui::Text(
        "API Version: %d.%d.%d",
        VK_VERSION_MAJOR(device_info.properties.apiVersion),
        VK_VERSION_MINOR(device_info.properties.apiVersion),
        VK_VERSION_PATCH(device_info.properties.apiVersion)
    );

    ImGui::Separator();

    //  Device extensions listbox
    static int device_ext_index = 0;
    extensions_listbox(
        "Device Extensions",
        device_info.extensions,
        device_ext_index
    );

    ImGui::Separator();

    //  Queue families listbox
    static int queue_family_index = 0;
    queue_families_listbox(device_info, queue_family_index);

    //  Queue family panel
    const VulkanQueueFamilyInfo& queue_family_info = device_info.queue_families.at(queue_family_index);
    queue_family_panel(queue_family_info);

    ImGui::Separator();

    device_features_panel(device_info);

    ImGui::Separator();
}

//  ----------------------------------------------------------------------------
std::function<void (engine::Game&)> make_vulkan_debug_panel_function(
    VkInstance instance
) {
    //  render_vk doesn't link to engine library so Game instance is unavailable
    return [instance](engine::Game& game) {
        vulkan_debug_panel(instance);
    };
}
}
