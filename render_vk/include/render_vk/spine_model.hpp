#pragma once

#include "render_vk/mesh.hpp"
#include "render_vk/vulkan_model.hpp"
#include <spine/spine.h>
#include <memory>
#include <string>

namespace render_vk
{
//  Stores attachment order for use with UBO data
struct AttachmentInfo
{
    uint16_t index;
    uint16_t slot;
    uint16_t skin;
    // uint16_t attachment;
};

struct SpineModel
{
    spine::SkeletonData* skeleton_data                         {nullptr};
    std::unique_ptr<spine::AnimationState> anim_state          {nullptr};
    std::unique_ptr<spine::AnimationStateData> anim_state_data {nullptr};
    std::unique_ptr<spine::Atlas> atlas                        {nullptr};
    std::unique_ptr<spine::Skeleton> skeleton;
    std::unique_ptr<spine::TextureLoader> texture_loader       {nullptr};
    std::vector<AttachmentInfo> attachment_infos;
    //  TODO: Meshes to load. Can discard after loading.
    std::vector<Mesh> meshes;
    VulkanModel model;
};
}
