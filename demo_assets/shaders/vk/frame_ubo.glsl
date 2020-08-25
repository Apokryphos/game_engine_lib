layout(set = 0, binding = 0) uniform FrameUniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 ortho_view;
    mat4 ortho_proj;
} frame_ubo;
