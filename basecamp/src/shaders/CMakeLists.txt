file(GLOB_RECURSE ALL_FILES "*.hlsl" "*.h")
file(GLOB_RECURSE CS_FILES "*.cs.hlsl")
file(GLOB_RECURSE VS_FILES "*.vs.hlsl")
file(GLOB_RECURSE PS_FILES "*.ps.hlsl")
file(GLOB_RECURSE RAY_FILES "*.ray.hlsl")
file(GLOB_RECURSE INC_FILES "*.inc.hlsl")


set(PROJECT_NAME shader_hlsl)
project(${PROJECT_NAME})

set(OUTPUT_FILES)
foreach(var ${ALL_FILES})
	get_filename_component(file_wle ${var} NAME_WLE)
	set(OUTPUT_FILENAME ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_wle})
	list(APPEND OUTPUT_FILES ${OUTPUT_FILENAME}.obj ${OUTPUT_FILENAME}.asm)
endforeach(var)
message(${OUTPUT_FILES})

add_custom_target(${PROJECT_NAME} SOURCES ${ALL_FILES} DEPENDS ${OUTPUT_FILES})
set_source_files_properties(${ALL_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

#find_program(FXC fxc
#	PATHS "$ENV{PROGRAMFILES\(X86\)}/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64")
set(DXC "${CMAKE_SOURCE_DIR}/ext/dxc/dxc.exe")

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
		PRE_BUILD 
		COMMAND del *.* /q
		COMMAND echo Cleaning files
		WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}
		VERBATIM
		)
#message("Cleaning files{\"\\n\", s8b}")

# DXC compiling
# https://simoncoenen.com/blog/programming/graphics/DxcCompiling	
macro(Dxc_compile src_files dxc_path profile spirv_opt)
	foreach(var ${src_files})
		get_filename_component(file_wle ${var} NAME_WLE)
		set(OUTPUT_FILENAME ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_wle})
		add_custom_command(
			OUTPUT ${OUTPUT_FILENAME}.obj ${OUTPUT_FILENAME}.asm
			PRE_BUILD 
			COMMAND ${dxc_path} -T ${profile} ${spirv_opt} -auto-binding-space 0 -Ni -Od -Fo ${OUTPUT_FILENAME}.obj -Fc ${OUTPUT_FILENAME}.asm -Zpr ${var}
			#COMMAND echo Effect Compiler ${var}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			#MAIN_DEPENDENCY ${var}
			DEPENDS ${var}
			COMMENT "Effect Compiler ${dxc_path} ${var}"
			)
		#message(${FXC} /T "vs_5_0" /Ni /Od /Fo ${OUTPUT_FILENAME}.obj /Fc ${OUTPUT_FILENAME}.asm ${var} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	endforeach(var)
endmacro()

Dxc_compile("${VS_FILES}" ${DXC} "vs_6_0" "")
Dxc_compile("${PS_FILES}" ${DXC} "ps_6_0" "")
Dxc_compile("${CS_FILES}" ${DXC} "cs_6_0" "")
Dxc_compile("${RAY_FILES}" ${DXC} "lib_6_5" "")

func_group_src("${ALL_FILES}")

set(PROJECT_NAME shader_spirv)
project(${PROJECT_NAME})

set(OUTPUT_FILES)
foreach(var ${ALL_FILES})
	get_filename_component(file_wle ${var} NAME_WLE)
	set(OUTPUT_FILENAME ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_wle})
	list(APPEND OUTPUT_FILES ${OUTPUT_FILENAME}.obj ${OUTPUT_FILENAME}.asm)
endforeach(var)
message(${OUTPUT_FILES})

add_custom_target(${PROJECT_NAME} SOURCES ${ALL_FILES} DEPENDS ${OUTPUT_FILES})

set_source_files_properties(${ALL_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

#find_program(FXC fxc
#	PATHS "$ENV{PROGRAMFILES\(X86\)}/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64")
#set(DXC "${CMAKE_SOURCE_DIR}/ext/dxc/dxc.exe")

find_package(Vulkan)
set(DXC "${Vulkan_INCLUDE_DIRS}/../bin/dxc.exe")


file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
		PRE_BUILD 
		COMMAND del *.* /q
		COMMAND echo Cleaning files
		WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}
		VERBATIM
		)
		
#set(SPIRV_OPTS "-spirv -fspv-reflect -fspv-target-env=\"vulkan1.1spirv1.4\"")
set(SPIRV_OPTS "-spirv -fspv-target-env=\"vulkan1.1spirv1.4\"")
message(${SPIRV_OPTS})

Dxc_compile("${VS_FILES}" ${DXC} "vs_6_0" ${SPIRV_OPTS})
Dxc_compile("${PS_FILES}" ${DXC} "ps_6_0" ${SPIRV_OPTS})
Dxc_compile("${CS_FILES}" ${DXC} "cs_6_0" ${SPIRV_OPTS})
Dxc_compile("${RAY_FILES}" ${DXC} "lib_6_5" ${SPIRV_OPTS})

func_group_src("${ALL_FILES}")
