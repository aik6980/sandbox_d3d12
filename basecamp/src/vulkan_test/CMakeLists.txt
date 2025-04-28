set(PROJECT_NAME vulkan_test)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.hpp" "*.cpp")
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)

find_package(Vulkan)

target_link_libraries(${PROJECT_NAME} common ${Vulkan_LIBRARY})

set(INC_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/src 
	${PROJECT_SOURCE_DIR} 
	${INC_ASSIMP}
	${Vulkan_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}/vma)
	
message(${Vulkan_INCLUDE_DIRS})
message(${Vulkan_LIBRARY})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DCOMPILE_CPP -DNOMINMAX)

if (MSVC)	
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
	message(${EXECUTABLE_OUTPUT_PATH})
endif(MSVC)

func_group_src("${PROJECT_FILES}")