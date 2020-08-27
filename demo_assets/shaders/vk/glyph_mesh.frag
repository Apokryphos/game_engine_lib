#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 1, binding = 0) uniform sampler2D texture_sampler[];

layout(location = 0) in vec3 frag_bg_color;
layout(location = 1) in vec3 frag_fg_color;
layout(location = 2) in vec2 frag_uv;
layout(location = 3) flat in uint frag_texture_id;

layout(location = 0) out vec4 out_color;

void main() {
    vec4 bg_color = vec4(frag_bg_color, 1.0f);
    vec4 fg_color = vec4(frag_fg_color, 1.0);
    vec4 tex_color = texture(texture_sampler[nonuniformEXT(frag_texture_id)], frag_uv);
    out_color = mix(bg_color, tex_color * fg_color, tex_color.a);
}
