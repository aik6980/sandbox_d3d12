set(PROJECT_NAME sandbox)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.hpp" "*.cpp")
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
target_link_libraries(${PROJECT_NAME} common engine_graphic)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}$<$<CONFIG:Debug>:_debug>")

set(INC_IMGUI ${CMAKE_SOURCE_DIR}/ext/imgui/include)
set(INC_OIS ${CMAKE_SOURCE_DIR}/ext/ois/include)
set(INC_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INC_IMGUI} ${INC_OIS} ${INC_ASSIMP})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DNOMINMAX -DCOMPILE_CPP)

if (MSVC)	
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
	message(${EXECUTABLE_OUTPUT_PATH})
endif(MSVC)

func_group_src("${PROJECT_FILES}")