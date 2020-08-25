#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "glyph_ubo.glsl"

layout(set = 1, binding = 0) uniform sampler2D texture_sampler[];

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in flat int instance_index;

layout(location = 0) out vec4 out_color;

void main() {
    Glyph glyph = glyph_ubo.glyphs[instance_index];
    vec4 bg_color = glyph.bg_color;
    vec4 fg_color = glyph.fg_color;
    vec4 tex_color = texture(texture_sampler[nonuniformEXT(glyph.texture_index)], frag_uv);
    out_color = mix(bg_color, fg_color * tex_color, tex_color.a);
}
