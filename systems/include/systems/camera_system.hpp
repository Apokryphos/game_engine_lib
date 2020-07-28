#pragma once

#include "ecs/entity_handle.hpp"
#include "ecs/entity_system.hpp"
#include "systems/system_ids.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace engine
{
class Game;
}

namespace systems
{
enum class CameraMode
{
    //  Camera position and target position are not adjusted
    Aim,
    //  Camera position is adjusted to maintain distance from target
    Orbit,
};

//  Camera component.
//  Requires the entity to have a position component.
struct CameraComponentData
{
    bool ortho;
    CameraMode mode;
    //  Distance between camera and target
    float distance;
    //  Direction to change zoom each frame
    float zoom_direction;
    //  Zoom speed
    float zoom_speed;
    //  Target position
    glm::vec3 target;
    //  Target entity
    ecs::EntityHandle target_entity;
    //  View matrix (not serialized)
    glm::mat4 view;

    template <typename Archive>
    void archive(Archive& ar) {
        ar(
            ortho,
            mode,
            distance,
            zoom_direction,
            zoom_speed,
            target,
            target_entity
        );
    }
};

class CameraSystem : public ecs::EntitySystem<CameraComponentData>
{
    ecs::EntityHandle m_active_camera;

    virtual void initialize_component_data(
        size_t index,
        CameraComponentData& data
    ) override;

public:
    CameraSystem(ecs::EcsRoot& ecs_root, unsigned int max_components)
    : EntitySystem(ecs_root, CameraSystem::Id, "camera_system", max_components) {
    }

    void activate(const Component cmpnt) {
        const ecs::Entity entity = get_entity_by_component_index(cmpnt.index);
        m_active_camera.set_entity(entity);
    }

    bool has_active_camera() const {
        return m_active_camera.has_entity();
    }

    ecs::Entity get_active_camera() const {
        return m_active_camera.get_entity();
    }

    glm::vec3 get_target(const Component cmpnt) const {
        return get_component_data(cmpnt).target;
    }

    ecs::EntityHandle& get_target_entity_handle(const Component cmpnt) {
        return get_component_data(cmpnt).target_entity;
    }

    glm::mat4 get_view_matrix(const Component cmpnt) const {
        return get_component_data(cmpnt).view;
    }

    static const common::SystemId Id = SYSTEM_ID_CAMERA;

    void set_mode(const Component cmpnt, CameraMode mode) {
        auto& data = get_component_data(cmpnt);
        get_component_data(cmpnt).mode = mode;

        if (mode == CameraMode::Orbit) {
            data.distance = 3.0f;
        }
    }

    void set_ortho(const Component cmpnt, bool ortho) {
        get_component_data(cmpnt).ortho = ortho;
    }

    void set_target(const Component cmpnt, const glm::vec3& target) {
        get_component_data(cmpnt).target = target;
    }

    void set_target_entity(const Component cmpnt, const ecs::Entity* target) {
        if (target != nullptr) {
            get_component_data(cmpnt).target_entity.set_entity(*target);
        } else {
            get_component_data(cmpnt).target_entity.reset();
        }
    }

    void update(engine::Game& game);

    void zoom_in(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.zoom_direction = std::clamp(-amount, -1.0f, 1.0f);
    }

    void zoom_out(const Component cmpnt, float amount) {
        auto& data = get_component_data(cmpnt);
        data.zoom_direction = std::clamp(amount, -1.0f, 1.0f);
    }
};
}
