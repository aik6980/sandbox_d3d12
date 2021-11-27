#pragma once

#include <assert.h>
#include <atltypes.h>
#include <windows.h>

#include <chrono>
#include <codecvt>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// d3d12
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <initguid.h>
#include <wrl.h>

#include "d3d12/D3D12MemoryAllocator/D3D12MemAlloc.h"
#include "d3d12/d3dx12/d3dx12.h"

// directx math
#include <DirectXColors.h>
#include <DirectXMath.h>

using namespace std;
using Microsoft::WRL::ComPtr;

#include "d3d12/input_layout_desc.h"
#include "d3d12/input_layout_element_desc_consts.h"
#include "d3d12/sampler_desc.h"
#include "d3d12/shader_reflection.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/imgui.h"

// Forward Declaration
namespace D3D12 {
struct MESH_LOCATION;

class Device;
class FrameResource;
class Resource_manager;
class Shader_manager;
class MESH_BUFFER;
struct Buffer;
struct Sampler;

class Technique;
class TechniqueInstance;

struct TechniqueInit {
    string m_vs;
    string m_ps;
};

} // namespace D3D12

class EngineClient {
  public:
    virtual void load_resource() = 0;
    virtual void update()        = 0;
    virtual void draw()          = 0;

    string m_name;
};
