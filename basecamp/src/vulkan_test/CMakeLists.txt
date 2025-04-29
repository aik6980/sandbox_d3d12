set(PROJECT_NAME vulkan_test)
project(${PROJECT_NAME})

find_package(Vulkan)
get_filename_component(VULKAN_SDK_DIR ${Vulkan_INCLUDE_DIRS} DIRECTORY)


file(GLOB_RECURSE PROJECT_FILES "*.h" "*.hpp" "*.cpp")
# add Spirv-Reflect source files to the project
set(SPIRV_REFLECT_DIR ${VULKAN_SDK_DIR}/Source/Spirv-Reflect)
file(GLOB_RECURSE SPIRV_REFLECT_FILES "${SPIRV_REFLECT_DIR}/*.h" "${SPIRV_REFLECT_DIR}/*.hpp" "${SPIRV_REFLECT_DIR}/*.c" "${SPIRV_REFLECT_DIR}/*.cpp")
message(${SPIRV_REFLECT_FILES})

add_executable(${PROJECT_NAME} WIN32 ${PROJECT_FILES} ${SPIRV_REFLECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
# Tell CMake to treat .c files as C++ files
set_source_files_properties(${SPIRV_REFLECT_FILES} PROPERTIES LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME} common ${Vulkan_LIBRARY})

set(INC_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/src 
	${PROJECT_SOURCE_DIR} 
	${INC_ASSIMP}
	${Vulkan_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}/vma
	${SPIRV_REFLECT_DIR}
	)
	
message(${Vulkan_INCLUDE_DIRS})
message(${Vulkan_LIBRARY})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DCOMPILE_CPP -DNOMINMAX)

if (MSVC)	
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
	message(${EXECUTABLE_OUTPUT_PATH})
endif(MSVC)

func_group_src("${PROJECT_FILES} ${SPIRV_REFLECT_FILES}")