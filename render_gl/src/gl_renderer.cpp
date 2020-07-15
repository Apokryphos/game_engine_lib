#include "common/log.hpp"
#include "platform/glfw_init.hpp"
#include "render_gl/gl_renderer.hpp"

using namespace common;
using namespace platform;
using namespace render;

namespace render_gl
{
//  ----------------------------------------------------------------------------
GlRenderer::GlRenderer()
: Renderer(RenderApi::OpenGl) {
}

//  ----------------------------------------------------------------------------
void GlRenderer::begin_frame() {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
void GlRenderer::draw_frame(GLFWwindow* glfw_window) {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
float GlRenderer::get_aspect_ratio() const {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
bool GlRenderer::initialize(GLFWwindow* glfw_window) {
    log_info("Initializing GL renderer...");

    //  Initialize glad
    if (!glad_init()) {
        return false;
    }

    throw std::runtime_error("Not implemented.");

    return true;
}

//  ----------------------------------------------------------------------------
void GlRenderer::load_model(
    AssetId id,
    const std::string& path
) {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
void GlRenderer::load_texture(
    AssetId id,
    const std::string& path
) {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
void GlRenderer::resize(GLFWwindow* glfw_window) {
    throw std::runtime_error("Not implemented.");
}

//  ----------------------------------------------------------------------------
void GlRenderer::shutdown() {
    log_debug("Shutting down GL renderer...");

    throw std::runtime_error("Not implemented.");
}
}
