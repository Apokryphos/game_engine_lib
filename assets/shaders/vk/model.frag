#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform FragmentPushConstants {
    layout(offset = 64) uint texture_index;
} fpc;

// layout(set = 1, binding = 0) uniform ObjectUniformBufferObject {
//     mat4 model;
//     uint texture_index;
// } obj_ubo;

layout(set = 1, binding = 1) uniform sampler2D texSampler[2];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // outColor = vec4(fragTexCoord, 0.0, 1.0);
    outColor = texture(texSampler[nonuniformEXT(fpc.texture_index)], fragTexCoord);
}
