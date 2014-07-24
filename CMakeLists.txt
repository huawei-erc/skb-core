cmake_minimum_required (VERSION 2.6)
project (Skb)

include(FindPkgConfig)

pkg_check_modules(GIO2 REQUIRED gio-2.0)
include_directories(${GIO2_INCLUDE_DIRS})

#This is cmake trick. The flags are populated as cmake list, e.g. ;-separated list.
#We got to replace semicolons with spaces to flatten that out into format linker will understand.
string (REPLACE ";" " " GIO_LDFLAGS "${GIO2_LDFLAGS}")
string (REPLACE ";" " " GIO_CFLAGS "${GIO2_CFLAGS}")

# The version number.
set (Skb_VERSION_MAJOR 1)
set (Skb_VERSION_MINOR 0)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -std=c99 -I/usr/local/eclipse/include/x86_64_linux -D __USE_SVID -D GLOBAL_DEBUG -I /usr/include/gio-unix-2.0 ${GIO_CFLAGS}")
set (GDBUS_CODEGEN "gdbus-codegen")

function(add_gdbus_codegen)
  set(_one_value OUTFILES NAME PREFIX NAMESPACE SERVICE_XML)
  set(_multi_value DEPENDS)
  cmake_parse_arguments (arg "" "${_one_value}" "${_multi_value}" ${ARGN})

  if(arg_PREFIX)
    set(PREFIX --interface-prefix ${arg_PREFIX})
  endif()
  
  if(arg_NAMESPACE)
    set(NAMESPACE --c-namespace ${arg_NAMESPACE})
  endif()
  
  add_custom_command(
    OUTPUT "${arg_NAME}.h" "${arg_NAME}.c"
    COMMAND "${GDBUS_CODEGEN}"
        --generate-c-code "${arg_NAME}"
        ${PREFIX}
        ${NAMESPACE}
        "${arg_SERVICE_XML}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${arg_DEPENDS} "${arg_SERVICE_XML}"
  )
  set(${arg_OUTFILES} ${${arg_OUTFILES}} "${CMAKE_CURRENT_BINARY_DIR}/${arg_NAME}.c" PARENT_SCOPE)
endfunction(add_gdbus_codegen)



add_gdbus_codegen(
OUTFILES SKB_GENERATED_SOURCES
NAME Skb
PREFIX org.freedesktop.
NAMESPACE  ""
SERVICE_XML "org.freedesktop.Skb.xml"
)




SET (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} $ENV{LDFLAGS}
    CACHE STRING "Flags used by the linker during the creation of .so files.")

SET (CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS_INIT} $ENV{LDFLAGS}
     CACHE STRING "Flags used by the linker during the creation of modules.")

SET( CMAKE_EXE_LINKER_FLAGS  "-lreadline -leclipse -L /usr/local/eclipse/lib/x86_64_linux ${GIO_LDFLAGS}" )


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/SkbConfig.h.in"
  "${PROJECT_BINARY_DIR}/SkbConfig.h"
  )


include_directories ("${PROJECT_SOURCE_DIR}/icparc_solver")
add_subdirectory (icparc_solver)
add_subdirectory (doc)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories ("${PROJECT_BINARY_DIR}")

# add the executable
add_executable (skb skb_service.c skb_functions.c skb_gdbus.c ${SKB_GENERATED_SOURCES})
target_link_libraries (skb  ${EXTRA_LIBS} icparc_solver)


# add the install targets
install (TARGETS skb DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/SkbConfig.h" 
  DESTINATION include)

