# - Try to find precompiled headers support for GCC 3.4 and 4.x
# Once done this will define:
#
# Variable:
#   PCHSupport_FOUND
#
# Macro:
#   ADD_PRECOMPILED_HEADER

IF(CMAKE_COMPILER_IS_GNUC)
    EXEC_PROGRAM(
	${CMAKE_C_COMPILER}
	ARGS 			--version
	OUTPUT_VARIABLE _compiler_output)
    STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"
	   gcc_compiler_version ${_compiler_output})
    #MESSAGE("GCC Version: ${gcc_compiler_version}")
    IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	SET(PCHSupport_FOUND TRUE)
    ELSE(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
	    SET(PCHSupport_FOUND TRUE)
	ENDIF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
    ENDIF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
ENDIF(CMAKE_COMPILER_IS_GNUC)

MACRO(ADD_PRECOMPILED_HEADER _targetName _input )

    GET_FILENAME_COMPONENT(_name ${_input} NAME)
    SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
    MAKE_DIRECTORY(${_outdir})
    SET(_output "${_outdir}/${CMAKE_BUILD_TYPE}")
    STRING(TOUPPER "CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
    SET(_compiler_FLAGS "${CMAKE_C_FLAGS} ${${_flags_var_name}}")

    GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
    FOREACH(item ${_directory_flags})
    LIST(APPEND _compiler_FLAGS -I"${item}")
    ENDFOREACH(item)

	GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
    LIST(APPEND _compiler_FLAGS ${_directory_flags})

    SEPARATE_ARGUMENTS(_compiler_FLAGS)
    #MESSAGE("_compiler_FLAGS: ${_compiler_FLAGS}")
    message("${CMAKE_C_COMPILER} ${_compiler_FLAGS} -x c-header -o ${_output} ${_source}")
    ADD_CUSTOM_COMMAND(
	OUTPUT ${_output}
	COMMAND ${CMAKE_C_COMPILER}
				${_compiler_FLAGS}
				-x c-header
				-o ${_output} ${_source}
	DEPENDS ${_source} )
	ADD_CUSTOM_TARGET(${_name}_gch DEPENDS ${_output})
    ADD_DEPENDENCIES(${_targetName} ${_name}_gch)
    #SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-include ${_name} -Winvalid-pch -H")
    #SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include ${_name} -Winvalid-pch")
    SET_TARGET_PROPERTIES(${_targetName} PROPERTIES
	COMPILE_FLAGS "-include ${_name} -Winvalid-pch"
    )

ENDMACRO(ADD_PRECOMPILED_HEADER)
