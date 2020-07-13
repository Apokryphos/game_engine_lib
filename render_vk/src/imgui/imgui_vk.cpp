#include "render_vk/command_buffer.hpp"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_vulkan.h"
#include <stdexcept>

namespace render_vk
{
//  ImGui requires a new descriptor pool to be created
static VkDescriptorPool s_descriptor_pool;

//  ----------------------------------------------------------------------------
static void create_imgui_descriptor_pool(
    VkDevice device,
    VkDescriptorPool& descriptor_pool
) {
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool)) {
        throw std::runtime_error("Failed to create ImGui descriptor pool.");
    }
}

//  ----------------------------------------------------------------------------
static void load_imgui_fonts(
    VkDevice device,
    VkQueue transfer_queue,
    VkCommandPool command_pool
) {
    VkCommandBuffer command_buffer = begin_single_time_commands(device, command_pool);
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
    end_single_time_commands(device, command_pool, command_buffer, transfer_queue);
}

//  ----------------------------------------------------------------------------
//  Initializes ImGui (called at start of application)
void imgui_vulkan_init(
    VkInstance instance,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue queue,
    VulkanSwapchain& swapchain,
    VkRenderPass render_pass,
    VkCommandPool command_pool
) {
    //  Create descriptor pool for ImGui
    create_imgui_descriptor_pool(device, s_descriptor_pool);

    const uint32_t image_count = swapchain.images.size();

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physical_device;
    init_info.Device = device;
    init_info.QueueFamily = 0;
    init_info.Queue = queue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = s_descriptor_pool;
    init_info.Allocator = nullptr;
    init_info.MinImageCount = image_count;
    init_info.ImageCount = image_count;
    init_info.CheckVkResultFn = nullptr;

    //  Initialize ImGui Vulkan implementation
    ImGui_ImplVulkan_Init(&init_info, render_pass);

    //  Upload fonts to GPU
    load_imgui_fonts(device, queue, command_pool);
}

//  ----------------------------------------------------------------------------
// void imgui_vulkan_recreate_swapchain(
//     VkInstance instance,
//     VkPhysicalDevice physical_device,
//     VkDevice device,
//     VkQueue queue,
//     VulkanSwapchain& swapchain,
//     VkRenderPass render_pass,
//     VkCommandPool command_pool
// ) {
//     //  Create new objects
//     imgui_vulkan_init(
//         instance,
//         physical_device,
//         device,
//         queue,
//         swapchain,
//         render_pass,
//         command_pool
//     );
// }

//  ----------------------------------------------------------------------------
void imgui_vulkan_render_frame(VkCommandBuffer command_buffer) {
    ImGui::Render();

    auto draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}

//  ----------------------------------------------------------------------------
void imgui_vulkan_cleanup_swapchain(VkDevice device) {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(device, s_descriptor_pool, nullptr);
    s_descriptor_pool = VK_NULL_HANDLE;

    //  End ImGui frame (empty frame)
    ImGui::EndFrame();
}

//  ----------------------------------------------------------------------------
void imgui_vulkan_shutdown(VkDevice device) {
    imgui_vulkan_cleanup_swapchain(device);
}
}
