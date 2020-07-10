#include "systems/editor/model_system_editor_panel.hpp"
#include "systems/model_system.hpp"
#include "imgui.h"

using namespace engine;

namespace systems
{
//  ----------------------------------------------------------------------------
ModelSystemEditorPanel::ModelSystemEditorPanel(ModelSystem& model_sys)
: EntitySystemEditorPanel(model_sys) {
}

//  ----------------------------------------------------------------------------
void ModelSystemEditorPanel::on_update(Game& game, const ecs::Entity entity) {
    ModelSystem& model_sys = get_system();
    const auto model_cmpnt = model_sys.get_component(entity);

    int model_id = model_sys.get_id(model_cmpnt);
    if (ImGui::InputInt("Model ID", &model_id)) {
        model_sys.set_id(model_cmpnt, model_id);
    }
}
}
