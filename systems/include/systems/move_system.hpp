#pragma once

#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"
#include <glm/vec3.hpp>
#include <algorithm>

namespace engine
{
class Game;
}

namespace systems
{
struct MoveComponentData
{
    //  Facing direction
    float direction;
    float move_speed;
    float turn_speed;
    float turn;
    //  Movement axis
    glm::vec3 move;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(direction, move_speed, turn, move);
    }
};

class MoveSystem : public ecs::EntitySystem<MoveComponentData>
{
    virtual void initialize_component_data(
        size_t index,
        MoveComponentData& data
    ) override;

public:
    MoveSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, MoveSystem::Id, "move_system", max_components) {
    }

    static const common::SystemId Id = SYSTEM_ID_MOVE;

    float get_direction(const Component cmpnt) const {
        return get_component_data(cmpnt).direction;
    }

    //  Gets a normalized vector representing current heading
    glm::vec3 get_facing(const Component cmpnt) const;

    float get_speed(const Component cmpnt) const {
        return get_component_data(cmpnt).move_speed;
    }

    void move_backward(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.move.y = std::clamp(-amount, -1.0f, 1.0f);
    }

    void move_forward(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.move.y = std::clamp(amount, -1.0f, 1.0f);
    }

    void move_left(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.move.x = std::clamp(-amount, -1.0f, 1.0f);
    }

    void move_right(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.move.x = std::clamp(amount, -1.0f, 1.0f);
    }

    void set_move_speed(const Component cmpnt, float move_speed) {
        get_component_data(cmpnt).move_speed = move_speed;
    }

    void set_turn_speed(const Component cmpnt, float turn_speed) {
        get_component_data(cmpnt).turn_speed = turn_speed;
    }

    void turn_left(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.turn = std::clamp(amount, -1.0f, 1.0f);
    }

    void turn_right(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.turn = std::clamp(-amount, -1.0f, 1.0f);
    }

    void update(engine::Game& game);
};
}
