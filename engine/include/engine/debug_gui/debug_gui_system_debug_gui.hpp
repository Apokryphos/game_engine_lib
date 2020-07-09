#pragma once

#include "engine/base_systems/debug_gui_system.hpp"
#include "engine/debug_gui/debug_gui.hpp"

namespace engine
{
class DebugGuiSystemDebugGui : public SystemDebugGui<DebugGuiSystem>
{
protected:
    virtual void on_update(Game& game) override;

public:
    DebugGuiSystemDebugGui(DebugGuiSystem& debug_gui_sys);
};
}
