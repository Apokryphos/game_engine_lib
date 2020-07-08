#include "demo/systems/demo_system.hpp"
#include <glm/glm.hpp>

namespace demo
{
//  ----------------------------------------------------------------------------
DemoSystem::DemoSystem()
: System(SYSTEM_ID_DEMO, "demo_system"),
  m_fov(45.0f),
  m_rotate(glm::radians(-135.0f)),
  m_position(0.0f, -2.0f, 2.0f) {
}

//  ----------------------------------------------------------------------------
void DemoSystem::forward(float amount) {
    m_position.y += amount;
}

//  ----------------------------------------------------------------------------
float DemoSystem::get_fov() const {
    return m_fov;
}

//  ----------------------------------------------------------------------------
glm::vec3 DemoSystem::get_position() const {
    return m_position;
}

//  ----------------------------------------------------------------------------
float DemoSystem::get_rotate() const {
    return m_rotate;
}

//  ----------------------------------------------------------------------------
void DemoSystem::rotate(float amount) {
    m_rotate += amount;
}

//  ----------------------------------------------------------------------------
void DemoSystem::strafe(float amount) {
    m_position.x += amount;
}

//  ----------------------------------------------------------------------------
void DemoSystem::zoom(float amount) {
    m_fov = std::clamp(m_fov + amount, 1.0f, 90.0f);
}

//  ----------------------------------------------------------------------------
void DemoSystem::update(engine::Game& game) {
}
}
