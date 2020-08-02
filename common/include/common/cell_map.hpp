#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace common
{
template <typename Coord, typename Cell>
class CellMap
{
    Coord m_size;
    std::vector<Cell> m_cells;

    static inline uint32_t get_count(const glm::ivec2& size) {
        return size.x * size.y;
    }

    static inline uint32_t get_count(const glm::ivec3& size) {
        return size.x * size.y * size.z;
    }

    static inline uint32_t get_index(
        const glm::ivec2& coord,
        const glm::ivec3& size) {
        return coord.x + (coord.y * size.x);
    }

    static inline uint32_t get_index(
        const glm::ivec3& coord,
        const glm::ivec3& size
    ) {
        return (coord.z * size.x * size.y) + (coord.y * size.x) + coord.x;
    }

    static inline bool check_bounds(
        const glm::ivec2& coord,
        const glm::ivec3& size
    ) {
        return (
            coord.x >= 0 &&
            coord.x < size.x &&
            coord.y >= 0 &&
            coord.y < size.y
        );
    }

    static inline bool check_bounds(
        const glm::ivec3& coord,
        const glm::ivec3& size
    ) {
        return (
            coord.x >= 0 &&
            coord.x < size.x &&
            coord.y >= 0 &&
            coord.y < size.y &&
            coord.z >= 0 &&
            coord.z < size.z
        );
    }

public:
    CellMap(Coord size)
    : m_size(size) {
        const uint32_t count = get_count(m_size);
        m_cells.resize(count);
        assert(m_cells.size() > 0);
    }

    inline Cell& get_cell(const Coord& coord) {
        const uint32_t index = get_index(coord, m_size);
        return m_cells.at(index);
    }

    inline const Cell& get_cell(const Coord& coord) const {
        const uint32_t index = get_index(coord, m_size);
        return m_cells.at(index);
    }

    inline const std::vector<Cell>& get_cells() const {
        return m_cells;
    }

    inline Coord get_size() const {
        return m_size;
    }

    inline bool in_bounds(const Coord& coord) const {
        return check_bounds(coord, m_size);
    }

    inline Cell* try_cell(const Coord& coord) {
        const uint32_t index = get_index(coord, m_size);
        if (index < m_cells.size()) {
            return &m_cells[index];
        }
        return nullptr;
    }

    inline const Cell* try_cell(const Coord& coord) const {
        const uint32_t index = get_index(coord, m_size);
        if (index < m_cells.size()) {
            return &m_cells[index];
        }
        return nullptr;
    }
};
}
