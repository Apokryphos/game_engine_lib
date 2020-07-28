#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform VertexPushConstants {
    layout(offset = 0) mat4 model;
    // layout(offset = 64) vec2 size;
} vpc;

layout(set = 0, binding = 0) uniform FrameUniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 ortho;
} frame_ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position =
        frame_ubo.ortho * vpc.model *
        (vec4(inPosition, 1.0) * vec4(100.0, 102.0, 1.0, 1.0));
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
