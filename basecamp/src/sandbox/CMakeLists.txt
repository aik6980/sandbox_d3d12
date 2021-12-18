project(sandbox)

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.hpp" "*.cpp")
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
target_link_libraries(${PROJECT_NAME} common engine_graphic)

include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/ext/ois/include)

ADD_DEFINITIONS(-DUNICODE -DNOMINMAX)
ADD_DEFINITIONS(-D_UNICODE)

if (MSVC)	
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(MSVC)

func_group_src("${PROJECT_FILES}")