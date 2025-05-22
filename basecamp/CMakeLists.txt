cmake_minimum_required(VERSION 3.22)
project(basecamp)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#create folder structure in Visual studio
function(func_group_src SRCS)
  #message(${SRCS})
  foreach(FILE ${SRCS}) 
    #convert source file to absolute
    get_filename_component(ABSOLUTE_PATH "${FILE}" ABSOLUTE)
    # Get the directory of the absolute source file
    get_filename_component(PARENT_DIR "${ABSOLUTE_PATH}" DIRECTORY)
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    # Group into "Source Files" and "Header Files"
    if("${GROUP}" STREQUAL "")
        set(GROUP " ")
    endif()
    #if ("${FILE}" MATCHES ".*\\.cpp")
    #  set(GROUP "Source Files${GROUP}")
    #elseif("${FILE}" MATCHES ".*\\.h")
    #  set(GROUP "Header Files${GROUP}")
    #endif()
    source_group("${GROUP}" FILES "${FILE}")
    #message(${GROUP})
  endforeach()
endfunction()


add_subdirectory(src/common)
add_subdirectory(src/engine_graphic)
add_subdirectory(src/sandbox)
add_subdirectory(src/shaders)

add_subdirectory(src/vulkan_test)