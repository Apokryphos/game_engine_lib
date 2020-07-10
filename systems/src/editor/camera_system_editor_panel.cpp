#include "systems/editor/camera_system_editor_panel.hpp"
#include "systems/camera_system.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
CameraSystemEditorPanel::CameraSystemEditorPanel(CameraSystem& camera_sys)
: EntitySystemEditorPanel(camera_sys) {
}

//  ----------------------------------------------------------------------------
void CameraSystemEditorPanel::on_update(Game& game, const ecs::Entity entity) {
    CameraSystem& camera_sys = get_system();

    const auto camera_cmpnt = camera_sys.get_component(entity);

    CameraComponentData data;
    camera_sys.get_component_data(camera_cmpnt, data);

    ImGui::InputFloat("Distance", &data.distance);

    camera_sys.set_component_data(camera_cmpnt, data);
}
}
