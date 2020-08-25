#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

namespace render
{
class GlyphBatch
{
public:
    struct Glyph
    {
        uint32_t texture_id;
        glm::vec2 size;
        glm::vec3 position;
        glm::vec4 bg_color;
        glm::vec4 fg_color;
    };

    struct Batch
    {
        uint32_t texture_id;
        size_t start;
        size_t end;
    };

private:
    std::vector<Glyph> m_glyphs;
    std::vector<Batch> m_batches;

public:
    void add_move(std::vector<Glyph>& glyphs) {
        m_glyphs = std::move(glyphs);

        if (m_glyphs.empty()) {
            return;
        }

        //  Sort glyphs by texture ID
        std::sort(
            m_glyphs.begin(),
            m_glyphs.end(),
            [](const Glyph& a, const Glyph& b) {
                return a.texture_id < b.texture_id;
            }
        );

        //  Create batches
        Batch* current_batch = nullptr;
        uint32_t texture_id = m_glyphs.front().texture_id - 1;
        for (size_t n = 0; n < m_glyphs.size(); ++n) {
            const Glyph& glyph = m_glyphs[n];

            if (glyph.texture_id != texture_id) {
                texture_id = glyph.texture_id;
                Batch batch { texture_id, n, n + 1 };
                m_batches.push_back(std::move(batch));
                current_batch = &m_batches.back();
            } else {
                current_batch->end = n + 1;
            }
        }
    }

    inline size_t empty() const {
        return m_glyphs.empty() || m_batches.empty();
    }

    inline const std::vector<Batch>& get_batches() const {
        return m_batches;
    }

    inline size_t get_instance_count() const {
        size_t instance_count = 0;
        for (const Batch& batch : m_batches) {
            instance_count += batch.end - batch.start;
        }
        return instance_count;
    }

    inline const std::vector<Glyph>& get_glyphs() const {
        return m_glyphs;
    }

    inline void remove_batches(
        std::function<bool (const Batch& batch)> predicate
    ) {
        m_batches.erase(
            std::remove_if(
                m_batches.begin(),
                m_batches.end(),
                predicate
            ),
            m_batches.end()
        );
    }
};
}
