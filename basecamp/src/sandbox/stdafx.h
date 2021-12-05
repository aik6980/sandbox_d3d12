#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <assert.h>
#include <atltypes.h>
#include <windows.h>

#include <codecvt>

#include "common/common.h"
#include "engine_graphic/engine_graphic.h"

// directx math
#include <DirectXColors.h>
#include <DirectXMath.h>

// OIS
#include "OISEvents.h"
#include "OISException.h"
#include "OISInputmanager.h"
#include "OISJoyStick.h"
#include "OISKeyboard.h"
#include "OISMouse.h"

// ECS
#include "entt/entt.hpp"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
