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
    vec4 color = texture(texSampler[nonuniformEXT(fpc.texture_index)], fragTexCoord);
    if (color.a <= 0) {
        discard;
    }

    outColor = color;
}
