#include "gfx_main.h"

std::unique_ptr<VKN::Device> g_gfx_device;

void Gfx_main::create(HINSTANCE hInstance, HWND hWnd)
{
    g_gfx_device = make_unique<VKN::Device>();
    g_gfx_device->create(hInstance, hWnd);
}

void Gfx_main::destroy() { g_gfx_device->destroy(); }

VKN::Device& Gfx_main::gfx_device() { return *g_gfx_device; }
VKN::Shader_manager& Gfx_main::shader_manager() { return *g_gfx_device->m_shader_manager; }
VKN::Resource_manager& Gfx_main::resource_manager() { return *g_gfx_device->m_resource_manager; }
