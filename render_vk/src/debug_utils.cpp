#include "common/log.hpp"
#include "render_vk/vulkan.hpp"
#include <vector>

using namespace common;

namespace render_vk
{
static bool s_debug_utils_exists = false;

static PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = 0;
static PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = 0;

static PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = 0;
static PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = 0;
static PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = 0;

static PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = 0;
static PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = 0;
static PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = 0;

static PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = 0;
static PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT​ = 0;

//  ----------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            log_debug("Vulkan: %s\n", pCallbackData->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            log_error("Vulkan: %s\n", pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

//  ----------------------------------------------------------------------------
void begin_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
) {
    #ifdef DEBUG
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkCmdBeginDebugUtilsLabelEXT(command_buffer, &info);
    #endif
}

//  ----------------------------------------------------------------------------
void begin_debug_marker(
    VkQueue queue,
    const char* name,
    const float color[4]
) {
    #ifdef DEBUG
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkQueueBeginDebugUtilsLabelEXT(queue, &info);
    #endif
}

//  ----------------------------------------------------------------------------
void end_debug_marker(VkCommandBuffer command_buffer) {
    #ifdef DEBUG
    pfn_vkCmdEndDebugUtilsLabelEXT(command_buffer);
    #endif
}

//  ----------------------------------------------------------------------------
void end_debug_marker(VkQueue queue) {
    #ifdef DEBUG
    pfn_vkQueueEndDebugUtilsLabelEXT(queue);
    #endif
}

//  ----------------------------------------------------------------------------
void insert_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
) {
    #ifdef DEBUG
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkCmdInsertDebugUtilsLabelEXT(command_buffer, &info);
    #endif
}

//  ----------------------------------------------------------------------------
void insert_debug_marker(
    VkQueue queue,
    const char* name,
    const float color[4]
) {
    #ifdef DEBUG
    VkDebugUtilsLabelEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pNext = NULL;
    info.pLabelName = name;
    memcpy(info.color, &color[0], sizeof(float) * 4);

    pfn_vkQueueInsertDebugUtilsLabelEXT(queue, &info);
    #endif
}

//  ----------------------------------------------------------------------------
void set_debug_name(
    VkDevice device,
    VkObjectType object_type,
    uint64_t object_handle,
    const char* name
) {
    #ifdef DEBUG
    if (pfn_vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT info{};
        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        info.objectType = object_type;
        info.objectHandle = object_handle;
        info.pNext = NULL;
        info.pObjectName = name;

        pfn_vkSetDebugUtilsObjectNameEXT(device, &info);
    }
    #endif
}

//  ----------------------------------------------------------------------------
bool check_debug_utils_support() {
    #ifdef DEBUG
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
    #endif

    return false;
}

//  ----------------------------------------------------------------------------
void init_vulkan_debug_utils(VkDevice device) {
    #ifdef DEBUG
    if (!s_debug_utils_exists) {
        return;
    }

    pfn_vkCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetDeviceProcAddr(
            device,
            "vkCreateDebugUtilsMessengerEXT"
        )
    );

    pfn_vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetDeviceProcAddr(
            device,
            "vkDestroyDebugUtilsMessengerEXT"
        )
    );

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
    #endif
}

//  ----------------------------------------------------------------------------
void make_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT& info
) {
    info = {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = debug_callback;
    info.pUserData = nullptr;
}

//  ----------------------------------------------------------------------------
void create_debug_messenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT& debug_messenger
) {
    #ifdef DEBUG
    VkDebugUtilsMessengerCreateInfoEXT msg_info{};
    make_debug_messenger_create_info(msg_info);

    if (vkCreateDebugUtilsMessengerEXT(instance, &msg_info, nullptr, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger.");
    }
    #endif
}
}
