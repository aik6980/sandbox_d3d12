file(GLOB_RECURSE CS_FILES "*.cs.hlsl")
file(GLOB_RECURSE VS_FILES "*.vs.hlsl")
file(GLOB_RECURSE PS_FILES "*.ps.hlsl")
file(GLOB_RECURSE INC_FILES "*.inc.hlsl")

set(PROJECT_NAME shader_hlsl)
project(${PROJECT_NAME})

add_custom_target(${PROJECT_NAME} SOURCES ${VS_FILES} ${PS_FILES} ${INC_FILES})

set_source_files_properties(${VS_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
set_source_files_properties(${PS_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
set_source_files_properties(${INC_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

#find_program(FXC fxc
#	PATHS "$ENV{PROGRAMFILES\(X86\)}/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64")
set(DXC "${CMAKE_SOURCE_DIR}/ext/dxc/dxc.exe")

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

# DXC compiling
# https://simoncoenen.com/blog/programming/graphics/DxcCompiling		
foreach(var ${VS_FILES})
	get_filename_component(file_wle ${var} NAME_WLE)
	set(OUTPUT_FILENAME ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_wle})
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${DXC} -T "vs_6_0" -Ni -Od -Fo ${OUTPUT_FILENAME}.obj -Fc ${OUTPUT_FILENAME}.asm -Zpr ${var}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Effect Compiler ${var} "
		)
	#message(${FXC} /T "vs_5_0" /Ni /Od /Fo ${OUTPUT_FILENAME}.obj /Fc ${OUTPUT_FILENAME}.asm ${var} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach(var)

foreach(var ${PS_FILES})
	get_filename_component(file_wle ${var} NAME_WLE)
	set(OUTPUT_FILENAME ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_wle})
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${DXC} -T "ps_6_0" -Ni -Od -Fo ${OUTPUT_FILENAME}.obj -Fc ${OUTPUT_FILENAME}.asm -Zpr ${var}
		COMMENT "Effect Compiler ${var} "
		)
endforeach(var)

set(PROJECT_NAME shader_spirv)
project(${PROJECT_NAME})

add_custom_target(${PROJECT_NAME} SOURCES ${CS_FILES} ${VS_FILES} ${PS_FILES})

set_source_files_properties(${CS_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
set_source_files_properties(${VS_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
set_source_files_properties(${PS_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

set(DXC "${CMAKE_SOURCE_DIR}/ext/dxc/dxc.exe")
set(GLSLANG "C:/VulkanSDK/1.2.135.0/Bin/glslangValidator.exe")

file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
foreach(var ${VS_FILES})
	get_filename_component(file_we ${var} NAME_WE)
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${DXC} -spirv /T "vs_6_0" /Ni /Od /Fo ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_we}.vs.spirv ${var}
		#COMMAND ${GLSLANG} -S "vert" -V -D -e "main" -o ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_we}.vs.spirv ${var}
		COMMENT "DXC Compiler ${var} \n"
		)
endforeach(var)

foreach(var ${PS_FILES})
	get_filename_component(file_we ${var} NAME_WE)
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${DXC} -spirv /T "ps_6_0" /Ni /Od /Fo ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_we}.ps.spirv ${var}
		#COMMAND ${GLSLANG} -S "frag" -V -D -e "main" -o ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}/${file_we}.ps.spirv ${var}
		COMMENT "DXC Compiler ${var} \n"
		)
endforeach(var)
