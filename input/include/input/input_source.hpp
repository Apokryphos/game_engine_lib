#pragma once

namespace input
{
//  A key, button, or axis on an input device
enum class InputSource
{
    None,
    Axis,
    Button,
    Key,
    MouseButton,
    MouseWheel,
};
}
