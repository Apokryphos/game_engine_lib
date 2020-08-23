#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace systems
{
struct GlyphComponentData
{
    uint16_t ch;
    uint32_t glyph_set_id;
    glm::vec4 fg;
    glm::vec4 bg;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            ch,
            glyph_set_id,
            fg,
            bg
        );
    }
};

class GlyphSystem : public ecs::EntitySystem<GlyphComponentData>
{
    struct GlyphSet
    {
        uint32_t texture_id;
        uint32_t width;
        uint32_t height;
    };

    std::vector<GlyphSet> m_glyph_sets;

public:
    GlyphSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, GlyphSystem::Id, "glyph_system", max_components) {
    }

    void add_glyph_set(
        const uint32_t texture_id,
        const uint32_t width,
        const uint32_t height
    ) {
        m_glyph_sets.push_back({ texture_id, width, height });
    }

    void set_bg(const Component cmpnt, const glm::vec4 bg) {
        get_component_data(cmpnt).bg = bg;
    }

    void set_fg(const Component cmpnt, const glm::vec4 fg) {
        get_component_data(cmpnt).fg = fg;
    }

    void set_glyph(const Component cmpnt, const char ch) {
        get_component_data(cmpnt).ch = ch;
    }

    glm::vec2 get_size(const Component cmpnt) const {
        const uint32_t glyph_set_id = get_component_data(cmpnt).glyph_set_id;
        const GlyphSet& glyph_set = m_glyph_sets.at(glyph_set_id);
        return { glyph_set.width, glyph_set.height };
    }

    uint32_t get_texture_id(const Component cmpnt) const {
        const auto& data = get_component_data(cmpnt);
        const uint32_t glyph = data.ch;
        const uint32_t glyph_set_id = data.glyph_set_id;
        const GlyphSet& glyph_set = m_glyph_sets.at(glyph_set_id);
        return glyph_set.texture_id + glyph;
    }

    static const common::SystemId Id = SYSTEM_ID_GLYPH;

    void set_glyph_set_id(const Component cmpnt, const uint32_t glyph_set_id)  {
        get_component_data(cmpnt).glyph_set_id = glyph_set_id;
    }
};
}
