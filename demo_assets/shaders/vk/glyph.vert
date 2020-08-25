#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "frame_ubo.glsl"
#include "glyph_ubo.glsl"

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out flat int instance_index;

void main() {
    Glyph glyph = glyph_ubo.glyphs[gl_InstanceIndex];
    mat4 mvp = frame_ubo.ortho_proj * frame_ubo.ortho_view * glyph.model;
    gl_Position = mvp * vec4(vertex_position, 1.0);

    frag_color = vertex_color;
    frag_uv = vertex_uv;
    instance_index = gl_InstanceIndex;
}
