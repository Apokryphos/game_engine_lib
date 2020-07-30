#pragma once

#include "common/asset.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <memory>
#include <mutex>

namespace render_vk
{
class VulkanModel;
class VulkanQueue;

class ModelManager
{
    using AssetId = common::AssetId;

    std::mutex m_models_mutex;
    std::map<common::AssetId, std::unique_ptr<VulkanModel>> m_models;
    std::unique_ptr<VulkanModel> m_billboard_quad;
    std::unique_ptr<VulkanModel> m_sprite_quad;

public:
    ModelManager();
    ~ModelManager();
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    void add_model(std::unique_ptr<VulkanModel> model);

    void initialize(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VkCommandPool command_pool
    );
    void load_model(
        const AssetId id,
        const std::string& path,
        VkPhysicalDevice physical_device,
        VkDevice device,
        VulkanQueue& graphics_queue,
        VkCommandPool command_pool
    );

    VulkanModel& get_billboard_quad();
    VulkanModel* get_model(const AssetId id);
    VulkanModel& get_sprite_quad();
    void unload(VkDevice device);
};
}
