#pragma once

#include "gfx_device/device.h"
#include "gfx_device/resource_manager.h"
#include "gfx_device/shader_manager.h"

class Gfx_main {
  public:
    static void create(HINSTANCE hInstance, HWND hWnd);
    static void destroy();

    static VKN::Device& gfx_device();
    static VKN::Shader_manager& shader_manager();
    static VKN::Resource_manager& resource_manager();
};
