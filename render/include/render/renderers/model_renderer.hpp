#pragma once

#include "render/model_batch.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
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
