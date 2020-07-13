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

    std::map<common::AssetId, std::unique_ptr<VulkanModel>> m_models;

public:
    ModelManager();
    ~ModelManager();

    void load_model(
        const AssetId id,
        const std::string& path,
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkQueue graphics_queue,
        VkCommandPool command_pool
    );

    VulkanModel* get_model(const AssetId id);
    void unload();
};
}
