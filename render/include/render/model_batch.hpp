#pragma once

#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

namespace render
{
struct ModelBatch
{
    uint32_t texture_id;
    uint32_t model_id;
    std::vector<glm::vec3> positions;
};
}
