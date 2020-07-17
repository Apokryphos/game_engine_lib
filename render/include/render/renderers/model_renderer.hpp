#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct ModelBatch
{
    uint32_t model_id;
    std::vector<glm::vec3> positions;
    std::vector<uint32_t> texture_ids;
};

class ModelRenderer
{
public:
    virtual void draw_models(
        const glm::mat4& view,
        const glm::mat4& proj,
        std::vector<ModelBatch>& batches
    ) = 0;
};
}
