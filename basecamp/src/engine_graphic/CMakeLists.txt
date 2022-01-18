set(PROJECT_NAME engine_graphic)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.cpp")
add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
set(LIB_DXC ${PROJECT_SOURCE_DIR}/d3d12/dxc/dxcompiler.lib)
target_link_libraries(${PROJECT_NAME} common d3d12 dxgi dxguid d3dcompiler ${LIB_DXC})

set(INC_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INC_ASSIMP})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DCOMPILE_CPP)

func_group_src("${PROJECT_FILES}")