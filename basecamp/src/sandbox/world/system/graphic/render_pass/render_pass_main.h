#pragma once

class Render_pass_main {
  public:
    void load_resource();

    void begin_render();
    void end_render();

    DXGI_FORMAT render_target_format();
    DXGI_FORMAT depth_stencil_format();

    weak_ptr<D3D12::Buffer> render_target_buffer();

  private:
    string              m_main_colour_buffer_id = "main_colour_buffer";
    CD3DX12_CLEAR_VALUE m_main_colour_buffer_clear_val;
};

class Render_pass_shadow_map {
  public:
    void load_resource();

    void begin_render();
    void end_render();

    DXGI_FORMAT render_target_format() { return DXGI_FORMAT_UNKNOWN; }
    DXGI_FORMAT depth_stencil_format() { return DXGI_FORMAT_D32_FLOAT; }

    weak_ptr<D3D12::Buffer> depth_stencil_buffer();

  private:
    string              m_depth_buffer_id        = "shadowmap_depth_buffer";
    CD3DX12_CLEAR_VALUE m_depth_buffer_clear_val = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
};

class Render_pass_raytrace_main {
  public:
    void                    load_resource();
    weak_ptr<D3D12::Buffer> render_target_buffer();

  private:
    string m_raytrace_colour_buffer_id = "raytrace_colour_buffer";
};
