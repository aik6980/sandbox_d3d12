#pragma once

#include <array>
#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <algorithm>
#include <assert.h>
#include <atltypes.h>
#include <windows.h>

#include <codecvt>

// d3d12
#include "d3d12/d3dx12/d3dx12.h"
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <initguid.h>
#include <wrl.h>

#include "d3d12/D3D12MemoryAllocator/D3D12MemAlloc.h"

// directx math
#include <DirectXColors.h>
#include <DirectXMath.h>

// common
#include "common/common.h"

using namespace std;
using Microsoft::WRL::ComPtr;
