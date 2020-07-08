#pragma once

#include "common/asset.hpp"
#include "render_vk/vulkan.hpp"
#include <map>
#include <memory>

namespace render_vk
{
class VulkanModel;

class ModelManager
{
    using AssetId = common::AssetId;

    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VkQueue m_graphics_queue;
    VkCommandPool m_command_pool;

    std::map<common::AssetId, std::unique_ptr<VulkanModel>> m_models;

public:
    ModelManager(
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkQueue graphics_queue,
        VkCommandPool command_pool
    );
    ~ModelManager();
    void load_model(const AssetId id, const std::string& path);
    VulkanModel* get_model(const AssetId id);
    void unload();
};
}
