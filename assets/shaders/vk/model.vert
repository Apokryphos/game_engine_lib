#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform FrameUniformBufferObject {
    mat4 view;
    mat4 proj;
} frame_ubo;

layout(binding = 1) uniform ModelUniformBufferObject {
    mat4 model;
    uint texture_index;
} obj_ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = frame_ubo.proj * frame_ubo.view * obj_ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor * (1 + obj_ubo.texture_index);
    fragTexCoord = inTexCoord;
}
