#pragma once

#include "render_vk/mesh.hpp"
#include "render_vk/vulkan_model.hpp"
#include <spine/spine.h>
#include <memory>
#include <string>

namespace render_vk
{
struct SpineModel
{
    spine::SkeletonData* skeleton_data                         {nullptr};
    std::unique_ptr<spine::AnimationStateData> anim_state_data {nullptr};
    std::unique_ptr<spine::Atlas> atlas                        {nullptr};
    std::unique_ptr<spine::Skeleton> skeleton;
    std::unique_ptr<spine::TextureLoader> texture_loader       {nullptr};
    Mesh mesh;
    VulkanModel model;
};
}
