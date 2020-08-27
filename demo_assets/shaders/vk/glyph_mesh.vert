#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "frame_ubo.glsl"

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_bg_color;
layout(location = 2) in vec3 vertex_fg_color;
layout(location = 3) in vec2 vertex_uv;
layout(location = 4) in uint vertex_texture_id;

layout(location = 0) out vec3 frag_bg_color;
layout(location = 1) out vec3 frag_fg_color;
layout(location = 2) out vec2 frag_uv;
layout(location = 3) out uint frag_texture_id;

void main() {
    mat4 mvp = frame_ubo.ortho_proj * frame_ubo.ortho_view;
    gl_Position = mvp * vec4(vertex_position, 1.0);
    frag_bg_color = vertex_bg_color;
    frag_fg_color = vertex_fg_color;
    frag_uv = vertex_uv;
    frag_texture_id = vertex_texture_id;
}
