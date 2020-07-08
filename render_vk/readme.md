# render_vk

Vulkan renderer.

## Dependencies

[tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)

## Known Issues

Initialization functions should return a boolean so a different renderer can
be used, but rendering functions should probably throw exceptions on error.

In **choose_swap_present_mode**, VK_PRESENT_MODE_MAILBOX_KHR is preferred, and
the first available present mode that matches it should be returned. However on
the development system (Fedora 32 / AMD RX 5600), when this present mode is used,
the display will black out when hitting the super key (activities overview),
alt-tabbing or switching between windows, or resizing the window. Currently
VK_PRESENT_MODE_FIFO_KHR is returned until the issue is resolved.

Avoid calling vkAllocateMemory for every individual buffer. Replace code in
vertex_buffer.cpp. See [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
and the conclusion section of [this tutorial](https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer).

Index and vertex buffers should be combined into one VkBuffer.
See [this recommendation](https://developer.nvidia.com/vulkan-memory-management)
and the conclusion section of [this tutorial](https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer).
