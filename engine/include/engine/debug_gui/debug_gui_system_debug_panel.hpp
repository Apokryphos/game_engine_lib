#pragma once

#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/debug_gui/system_debug_panel.hpp"

namespace engine
{
class DebugGuiSystemDebugPanel : public SystemDebugPanel<DebugGuiSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    DebugGuiSystemDebugPanel(DebugGuiSystem& debug_gui_sys);
};
}
