#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
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

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#define SPIRV_REFLECT_ENABLE_ASSERTS
#include "spirv_reflect.h"
