#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform FragmentPushConstants {
    layout(offset = 64) uint texture_index;
} fpc;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 bg_color = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 fg_color = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 tex_color = texture(texSampler[nonuniformEXT(fpc.texture_index)], fragTexCoord);
    outColor = mix(bg_color, fg_color * tex_color, tex_color.a);
}
