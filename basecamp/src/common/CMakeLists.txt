set(PROJECT_NAME common)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.cpp")
add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
set(LIB_OIS ${CMAKE_SOURCE_DIR}/ext/ois/OIS_d.lib)
target_link_libraries(${PROJECT_NAME} ${LIB_OIS})

set(INC_OIS ${CMAKE_SOURCE_DIR}/ext/ois/include)
include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INC_OIS})

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

# don't need this in CMAKE 3.22
#if (MSVC)
#	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
#	set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")	
#endif(MSVC)

func_group_src("${PROJECT_FILES}")