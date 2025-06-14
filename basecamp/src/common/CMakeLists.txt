set(PROJECT_NAME common)
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "*.h" "*.cpp")
add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)

add_library(LIB_OIS STATIC IMPORTED)
set_target_properties(LIB_OIS PROPERTIES
    IMPORTED_LOCATION_DEBUG "${CMAKE_SOURCE_DIR}/ext/ois/OIS_d.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_SOURCE_DIR}/ext/ois/OIS.lib"
)

add_library(LIB_ASSIMP STATIC IMPORTED)
set_target_properties(LIB_ASSIMP PROPERTIES
    IMPORTED_LOCATION_DEBUG "${CMAKE_SOURCE_DIR}/assimp/assimp-vc143-mtd.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_SOURCE_DIR}/assimp/assimp-vc143-mt.lib"
)

#set(LIB_OIS ${CMAKE_SOURCE_DIR}/ext/ois/OIS_d.lib)
#set(LIB_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/assimp-vc143-mt.lib)

target_link_libraries(${PROJECT_NAME} ${LIB_OIS} ${LIB_ASSIMP})

set(INC_OIS ${CMAKE_SOURCE_DIR}/ext/ois/include)
set(INC_ASSIMP ${CMAKE_SOURCE_DIR}/ext/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INC_OIS} ${INC_ASSIMP})

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DCOMPILE_CPP -DNOMINMAX)

# don't need this in CMAKE 3.22
#if (MSVC)
#	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
#	set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")	
#endif(MSVC)

func_group_src("${PROJECT_FILES}")