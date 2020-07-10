#include "demo/systems/demo_system.hpp"

using namespace engine;

namespace demo
{
//  ----------------------------------------------------------------------------
DemoSystem::DemoSystem()
: System(SYSTEM_ID_DEMO, "demo_system") {
}

//  ----------------------------------------------------------------------------
void DemoSystem::update(Game& game) {
}
}
