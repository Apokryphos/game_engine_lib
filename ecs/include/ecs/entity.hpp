#pragma once

#include <cstdint>
#include <functional>

namespace ecs
{
using EntityId = uint32_t;

const unsigned ENTITY_INDEX_BITS = 22;
const unsigned ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

const unsigned ENTITY_GENERATION_BITS = 8;
const unsigned ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

struct Entity
{
    EntityId id;

    Entity() {}

    explicit Entity(const EntityId id) {
        EntityId index = id & ENTITY_INDEX_MASK;
        EntityId generation = (id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK;
        this->id = (generation << ENTITY_INDEX_BITS) | index;
    }

    explicit Entity(const EntityId index, const EntityId generation) {
        this->id = (generation << ENTITY_INDEX_BITS) | index;
    }

    inline EntityId index() const {
        return this->id & ENTITY_INDEX_MASK;
    }

    inline EntityId generation() const {
        return (this->id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK;
    }

    template <typename Archive>
    void serialize(Archive& ar) {
        ar(id);
    }
};

inline bool operator==(const Entity entity1, const Entity entity2) {
    return entity1.id == entity2.id;
}

inline bool operator!=(const Entity entity1, const Entity entity2) {
    return entity1.id != entity2.id;
}

inline bool operator<(const Entity entity1, const Entity entity2) {
    return entity1.id < entity2.id;
}
}

namespace std
{
template <>
class hash<ecs::Entity>
{
public:
    std::size_t operator()(const ecs::Entity entity) const {
        return std::hash<unsigned>()(entity.id);
    }
};
}
