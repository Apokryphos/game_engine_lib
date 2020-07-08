# game_engine

C++ game engine.


## Projects

The name of each library matches its namespace.

| Project    | Description             |
| ---------- | ----------------------- |
| common     | Utility library         |
| demo       | Example application     |
| ecs        | Entity-component-system |
| engine     | Engine framework        |
| extlibs    | External dependencies   |
| filesystem | Game filesystem         |
| input      | Input devices           |
| platform   | Window and OS (GLFW)    |
| render     | Rendering               |
| render_gl  | OpenGL renderer         |
| render_vk  | Vulkan renderer         |
| systems    | Game systems            |

**engine** is the top-most library.


## Demo

The demo project can be used as a template for a new application.


## Dependencies

| Dependency | Description |
| ---------- | ----------- |
| [cereal](https://github.com/USCiLab/cereal) | Serialization |
| [glad](https://github.com/Dav1dde/glad) | OpenGL loader |
| [GLFW](https://github.com/glfw/glfw) | Window and input |
| [GLM](https://github.com/g-truc/glm) | Math |
| [ImGui](https://github.com/ocornut/imgui) | Debug/Editor GUI |
| [lodepng](https://github.com/lvandeve/lodepng) | PNG images |
| [rang](https://github.com/agauniyal/rang) | Log text colors |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | OBJ models |
| [volk](https://github.com/zeux/volk) | Vulkan loader |
| [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) | Vulkan API |
