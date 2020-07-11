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
    std::string name;
    VkPhysicalDevice device;
    std::vector<VulkanQueueFamilyInfo> queue_families;
};

struct VulkanInfo
{
    std::vector<VulkanPhysicalDeviceInfo> devices;
};

//  ----------------------------------------------------------------------------
static void init_vulkan_device_info(
    VkInstance instance,
    VulkanPhysicalDeviceInfo& info
) {
    const VkPhysicalDevice device = info.device;

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
        device_info.name = device_properties.deviceName;

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

        *out_text = devices.at(index).name.c_str();
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

    //  Physical devices listbox
    static int device_index = 0;
    device_listbox(info, device_index);

    ImGui::Separator();

    //  Queue families listbox
    const VulkanPhysicalDeviceInfo& device_info = info.devices.at(device_index);
    static int queue_family_index = 0;
    queue_families_listbox(device_info, queue_family_index);

    //  Queue family panel
    const VulkanQueueFamilyInfo& queue_family_info = device_info.queue_families.at(queue_family_index);
    queue_family_panel(queue_family_info);

    ImGui::Separator();
}

//  ----------------------------------------------------------------------------
std::function<void (engine::Game&)> make_vulkan_debug_panel_function(
    VkInstance instance
) {
    //  render_vk doesn't link to engine library so Game instance is unavailable
    return [instance](engine::Game& game) {
        if (ImGui::Begin("vulkan_debug")) {
            vulkan_debug_panel(instance);
        }

        ImGui::End();
    };
}
}
