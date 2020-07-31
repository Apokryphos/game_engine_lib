#pragma once

#include "render_vk/vulkan.hpp"
#include <array>
#include <cstdint>

namespace render_vk
{
inline float hex_color(uint8_t value) {
    return value / 255.0f;
}

const float DEBUG_MARKER_COLOR_BLUE[]     = { hex_color(0x41), hex_color(0x80), hex_color(0xa0), 1.0f };
const float DEBUG_MARKER_COLOR_GREEN[]    = { hex_color(0x7b), hex_color(0xb2), hex_color(0x4e), 1.0f };
const float DEBUG_MARKER_COLOR_ORANGE[]   = { hex_color(0xe8), hex_color(0x99), hex_color(0x73), 1.0f };
const float DEBUG_MARKER_COLOR_PURPLE[]   = { hex_color(0x2a), hex_color(0x23), hex_color(0x49), 1.0f };
const float DEBUG_MARKER_COLOR_RED[]      = { hex_color(0x7d), hex_color(0x38), hex_color(0x40), 1.0f };
const float DEBUG_MARKER_COLOR_YELLOW[]   = { hex_color(0xfb), hex_color(0xdf), hex_color(0x9b), 1.0f };

bool check_debug_utils_support();
void create_debug_messenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT& debug_messenger
);
void init_vulkan_debug_utils(VkDevice device);
void make_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT& info
);
void begin_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
);
void end_debug_marker(VkCommandBuffer command_buffer);

//  Use VulkanQueue::begin_debug_marker for shared queues.
void begin_debug_marker(VkQueue queue, const char* name, const float color[4]);
//  Use VulkanQueue::end_debug_marker for shared queue.
void end_debug_marker(VkQueue queue);
// void insert_debug_marker(
//     VkQueue queue,
//     const char* name,
//     const float color[4]
// );

void insert_debug_marker(
    VkCommandBuffer command_buffer,
    const char* name,
    const float color[4]
);

void set_debug_name(
    VkDevice device,
    VkObjectType object_type,
    uint64_t object_handle,
    const char* name
);

template <typename T>
inline void set_debug_name(
    VkDevice device,
    VkObjectType object_type,
    T object_handle,
    const char* name
) {
    set_debug_name(
        device,
        object_type,
        reinterpret_cast<uint64_t&>(object_handle),
        name
    );
}
}
