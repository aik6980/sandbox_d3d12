#pragma once

#include "engine_graphic.h"

class Engine {
  public:
    struct InitData {
        HWND HWnd;
    };

    static const int32_t VERSION = 1;

    int32_t get_version() { return VERSION; }

    void init(const InitData& initData);

    void load_resource();
    void destroy();

    void update();
    void draw();

    void register_client(unique_ptr<EngineClient> client);

    D3D12::Device&           render_device() { return *m_render_device; };
    D3D12::Resource_manager& resource_mgr();
    D3D12::Shader_manager&   shader_mgr() { return *m_shader_mgr; }

  private:
    // sub systems;
    unique_ptr<D3D12::Device>         m_render_device;
    unique_ptr<D3D12::Shader_manager> m_shader_mgr;

    // client
    unordered_map<string, unique_ptr<EngineClient>> m_engine_client_list;
};
