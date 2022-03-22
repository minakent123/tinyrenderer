function(print_all_variables)
    get_cmake_property(VARS VARIABLES)
    foreach(VAR ${VARS})
        message(STATUS "${VAR}=${${VAR}}")
    endforeach()
endfunction()

macro(get_srcs TARGET_DIRS OUT_SRCS)
    foreach(TARGET_DIR ${TARGET_DIRS})
        file(GLOB_RECURSE SRCS
             ${TARGET_DIR}/*.cpp
        )
        list(APPEND ${OUT_SRCS} ${SRCS})
    endforeach()
endmacro()

macro(configure_msvc_runtime)
    if(MSVC)
        set(VARS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
        foreach(VAR ${VARS})
            if(${VAR} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${VAR} "${${VAR}}")
            endif()
        endforeach()
    endif()
endmacro()

macro(set_compile_options TARGET)
    if(MSVC)
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endmacro()

function(set_common_compile_definitions TARGET)
	if(MSVC)
		target_compile_definitions(${TARGET} PRIVATE
			OS_WINDOWS
			_WIN64
			_CONSOLE
			_HAS_EXCEPTIONS=0
		)
	endif()
endfunction()

macro(run_code_format TARGET)
	if(MSVC)
		add_custom_command(
			TARGET ${TARGET}
			PRE_BUILD
			COMMAND ${CMAKE_SOURCE_DIR}/scripts/code_format.bat
			COMMENT "run code_format.bat"
		)
	endif()
endmacro()
