#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform VertexPushConstants {
    layout(offset = 0) mat4 model;
    // layout(offset = 64) vec2 size;
} vpc;

layout(set = 0, binding = 0) uniform FrameUniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 ortho_view;
    mat4 ortho_proj;
} frame_ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    mat4 view = frame_ubo.view;
    vec3 camera_right   = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up      = vec3(view[0][1], view[1][1], view[2][1]);
    vec3 camera_forward = vec3(view[0][2], view[1][2], view[2][2]);

    vec3 center = inPosition;

    vec3 pos =
        // inPosition +
        (camera_right   * center.x) +
        (camera_forward * center.y) +
        (camera_up      * center.z);

    gl_Position = frame_ubo.proj * frame_ubo.view * vpc.model * vec4(pos, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
