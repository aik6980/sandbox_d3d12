#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <assert.h>
#include <atltypes.h>
#include <windows.h>

// d3d12
#include <wrl.h>

// directx math
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace std;

using Microsoft::WRL::ComPtr;

// directx math
using namespace DirectX;
using namespace DirectX::PackedVector;

#include "debug/debug_output.h"
#include "debug/debug_util.h"

#include "helper/helper.h"
#include "helper/math.h"

#include "graphic/graphic.h"
