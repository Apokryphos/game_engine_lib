#pragma once

#include "render/model_batch.hpp"
#include "render/spine_sprite_batch.hpp"
#include "render/renderer.hpp"
#include "render_vk/color_image.hpp"
#include "render_vk/descriptor_set_layout.hpp"
#include "render_vk/depth.hpp"
#include "render_vk/frame_objects.hpp"
#include "render_vk/vulkan.hpp"
#include "render_vk/vulkan_asset_task_manager.hpp"
#include "render_vk/vulkan_swapchain.hpp"
#include <memory>
#include <vector>

struct GLFWwindow;

namespace render_vk
{
class BillboardRenderer;
class DescriptorSetManager;
class GlyphRenderer;
class ModelManager;
class ModelRenderer;
class RenderTaskManager;
class SpriteRenderer;
class SpineSpriteRenderer;
class TextureManager;
class VulkanSpineManager;

class VulkanRenderSystem : public render::Renderer
{
public:
    //  Frame objects for main thread
    struct Frame
    {
        FrameSyncObjects sync;
        FrameCommandObjects command;
    };

    enum class FrameStatus
    {
        None,
        //  Waiting for render threads to complete.
        Busy,
        //  Render threads are complete.
        Ready,
        //  Frame was discarded due to swapchain recreation or worker thread
        //  failure.
        Discarded,
    };

private:
    //  Status of current frame.
    FrameStatus m_frame_status          = FrameStatus::None;

    //  Recreate swapchain objects when true.
    bool m_framebuffer_resized          = false;

    //  The total number of frames (resources).
    uint8_t m_frame_count               = 3;
    //  The current frame number (0...frame count).
    uint8_t m_current_frame             = 0;
    //  Swapchain image index
    uint32_t m_image_index              = 0;

    uint32_t m_max_objects {0};

    VkSampleCountFlagBits m_msaa_samples {VK_SAMPLE_COUNT_1_BIT};

    VkInstance m_instance               = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device  = VK_NULL_HANDLE;
    VkDevice m_device                   = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface              = VK_NULL_HANDLE;
    //  Render pass
    VkRenderPass m_render_pass          = VK_NULL_HANDLE;

    //  Command pool used to create main thread resources
    VkCommandPool m_resource_command_pool = VK_NULL_HANDLE;

    DescriptorSetLayouts m_descriptor_set_layouts;

    //  Debug messenger
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

    GLFWwindow* m_glfw_window = nullptr;

    //  Graphics queue (may be the same as present queue)
    std::shared_ptr<VulkanQueue> m_graphics_queue;
    //  Presentation queue (may be the same as graphics queue)
    std::shared_ptr<VulkanQueue> m_present_queue;

    //  Swapchain
    VulkanSwapchain m_swapchain;
    //  Multisample color image buffer (MSAA)
    ColorImage m_color_image;
    //  Depth buffer image
    DepthImage m_depth_image;

    //  Main thread frame objects
    std::vector<Frame> m_frames;

    std::unique_ptr<ModelManager> m_model_mgr;
    std::shared_ptr<VulkanSpineManager> m_spine_mgr;
    std::unique_ptr<TextureManager> m_texture_mgr;

    std::shared_ptr<VulkanAssetTaskManager> m_asset_task_mgr;
    //  Manages rendering task worker threads
    std::unique_ptr<RenderTaskManager> m_render_task_mgr;
    std::unique_ptr<DescriptorSetManager> m_descriptor_set_mgr;

    //  Renderers
    std::unique_ptr<BillboardRenderer> m_billboard_renderer;
    std::unique_ptr<GlyphRenderer> m_glyph_renderer;
    std::unique_ptr<ModelRenderer> m_model_renderer;
    std::unique_ptr<SpineSpriteRenderer> m_spine_sprite_renderer;
    std::unique_ptr<SpriteRenderer> m_sprite_renderer;

    //  Checks the status of the current frame's rendering tasks.
    //  This function should only be called from end_frame.
    bool check_render_tasks_complete();
    //  Creates frame objects.
    void create_frame_resources();
    //  Creates swapchain and render pass.
    void create_swapchain_objects();
    void create_swapchain_dependents();
    void destroy_swapchain();
    void destroy_frame_resources();
    //  Recreates swapchain and dependent objects.
    void recreate_swapchain();
    //  Releases objects.
    void shutdown();

public:
    VulkanRenderSystem(const uint32_t max_objects);
    ~VulkanRenderSystem();
    //  Starts a new frame.
    virtual void begin_frame() override;
    std::shared_ptr<VulkanAssetTaskManager> get_asset_task_manager();
    std::shared_ptr<VulkanSpineManager> get_spine_manager();
    virtual void draw_billboards(
        std::vector<render::SpriteBatch>& batches
    ) override;
    virtual void draw_glyph_mesh(assets::AssetId glyph_mesh_id) override;
    virtual void draw_glyphs(render::GlyphBatch& glyph_batch) override;
    virtual void draw_models(
        std::vector<render::ModelBatch>& batches
    ) override;
    virtual void draw_spines(
        std::vector<render::SpineSpriteBatch>& batches
    ) override;
    virtual void draw_sprites(
        std::vector<render::SpriteBatch>& batches
    ) override;
    //  Presents the completed frame.
    virtual void end_frame() override;
    virtual float get_aspect_ratio() const override;
    virtual glm::vec2 get_size() const override;

    VkInstance get_instance() const {
        return m_instance;
    }

    //  Initializes objects.
    virtual bool initialize(GLFWwindow* glfw_window) override;
    //  Framebuffer was resized
    virtual void resize() override;
    virtual void update_frame_uniforms(
        const glm::mat4& view,
        const glm::mat4& proj,
        const glm::mat4& ortho_view,
        const glm::mat4& ortho_proj
    ) override;
};
}
