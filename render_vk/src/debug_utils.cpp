#include "common/log.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

using namespace common;

namespace render_vk
{
static bool s_debug_utils_exists = false;

static PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = 0;
static PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = 0;
static PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = 0;

static PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = 0;
static PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = 0;
static PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = 0;

static PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = 0;
static PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT​ = 0;

//  ----------------------------------------------------------------------------
void begin_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
) {
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkCmdBeginDebugUtilsLabelEXT(command_buffer, &info);
}

//  ----------------------------------------------------------------------------
void begin_debug_marker(
    VkQueue queue,
    const char* name,
    const float color[4]
) {
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkQueueBeginDebugUtilsLabelEXT(queue, &info);
}

//  ----------------------------------------------------------------------------
void end_debug_marker(VkCommandBuffer command_buffer) {
    pfn_vkCmdEndDebugUtilsLabelEXT(command_buffer);
}

//  ----------------------------------------------------------------------------
void end_debug_marker(VkQueue queue) {
    pfn_vkQueueEndDebugUtilsLabelEXT(queue);
}

//  ----------------------------------------------------------------------------
void insert_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
) {
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkCmdInsertDebugUtilsLabelEXT(command_buffer, &info);
}

//  ----------------------------------------------------------------------------
void insert_debug_marker(
    VkQueue queue,
    const char* name,
    const float color[4]
) {
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkQueueInsertDebugUtilsLabelEXT(queue, &info);
}

//  ----------------------------------------------------------------------------
void set_debug_name(
    VkDevice device,
    VkObjectType object_type,
    uint64_t object_handle,
    const char* name
) {
    if (pfn_vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT info{};
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        info.objectType = object_type;
        info.objectHandle = object_handle;
        info.pNext = NULL;
        info.pObjectName = name;

        pfn_vkSetDebugUtilsObjectNameEXT(device, &info);
    }
}

//  ----------------------------------------------------------------------------
bool check_debug_utils_support() {
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        nullptr
    );

    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        extensions.data()
    );

    for (auto& ext : extensions) {
        if (!strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            log_debug("%s extension found.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            s_debug_utils_exists = true;
            return true;
        }
    }

    s_debug_utils_exists = false;
    log_debug("%s extension not found.", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return false;
}

//  ----------------------------------------------------------------------------
void init_vulkan_debug_utils(VkDevice device) {
    if (!s_debug_utils_exists) {
        pfn_vkSetDebugUtilsObjectNameEXT = 0;
        return;
    }

    pfn_vkCmdBeginDebugUtilsLabelEXT =
        (PFN_vkCmdBeginDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkCmdBeginDebugUtilsLabelEXT"
        )
    );

    pfn_vkCmdEndDebugUtilsLabelEXT =
        (PFN_vkCmdEndDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkCmdEndDebugUtilsLabelEXT"
        )
    );

    pfn_vkCmdInsertDebugUtilsLabelEXT =
        (PFN_vkCmdInsertDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkCmdInsertDebugUtilsLabelEXT"
        )
    );

    pfn_vkQueueBeginDebugUtilsLabelEXT =
        (PFN_vkQueueBeginDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkQueueBeginDebugUtilsLabelEXT"
        )
    );

    pfn_vkQueueEndDebugUtilsLabelEXT =
        (PFN_vkQueueEndDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkQueueEndDebugUtilsLabelEXT"
        )
    );

    pfn_vkQueueInsertDebugUtilsLabelEXT =
        (PFN_vkQueueInsertDebugUtilsLabelEXT)(vkGetDeviceProcAddr(
            device,
            "vkQueueInsertDebugUtilsLabelEXT"
        )
    );

    pfn_vkSetDebugUtilsObjectNameEXT =
        (PFN_vkSetDebugUtilsObjectNameEXT)(vkGetDeviceProcAddr(
            device,
            "vkSetDebugUtilsObjectNameEXT"
        )
    );

    pfn_vkSetDebugUtilsObjectTagEXT​ =
        (PFN_vkSetDebugUtilsObjectTagEXT)(vkGetDeviceProcAddr(
            device,
            "vkSetDebugUtilsObjectTagEXT​"
        )
    );
}
}
