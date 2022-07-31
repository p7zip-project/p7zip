PROJECT(Lizard C)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lizard compression library")
set(CPACK_PACKAGE_VERSION_MAJOR 2)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(VERSION_STRING	" \"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}\" ")
include(CPack)

cmake_minimum_required (VERSION 2.6)
INCLUDE (CheckTypeSize)
check_type_size("void *" SIZEOF_VOID_P)
IF(SIZEOF_VOID_P STREQUAL  "8")
    set (CMAKE_SYSTEM_PROCESSOR "64bit")
    MESSAGE( STATUS "64 bit architecture detected size of void * is " ${SIZEOF_VOID_P})
ENDIF()

option(BUILD_TOOLS "Build the command line tools" ON)
option(BUILD_LIBS  "Build the libraries in addition to the tools" ON)
option(LINK_TOOLS_WITH_LIB "Link the command line tools with the (shared) library" OFF)

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_C_COMPILER_ID STREQUAL "Clang")
  SET(GNU_COMPATIBLE_COMPILER 1)
ENDIF()


set(LIB_DIR ../lib/)
set(PRG_DIR ../programs/)
set(ENTROPY_DIR ../lib/entropy/)
set(LIZARD_SRCS_LIB ${LIB_DIR}lizard_decompress.c ${LIB_DIR}lizard_compress.c ${LIB_DIR}lizard_compress.h ${LIB_DIR}lizard_common.h ${LIB_DIR}lizard_frame.c ${LIB_DIR}lizard_frame.h ${LIB_DIR}xxhash/xxhash.c)
set(LIZARD_SRCS ${LIB_DIR}lizard_frame.c ${LIB_DIR}xxhash/xxhash.c ${PRG_DIR}bench.c ${PRG_DIR}datagen.c ${PRG_DIR}lizardcli.c ${PRG_DIR}lizardio.c)
#set(ENTROPY_LIB_SRCS ${ENTROPY_DIR}entropy_common.c ${ENTROPY_DIR}fse_compress.c ${ENTROPY_DIR}fse_decompress.c ${ENTROPY_DIR}huf_compress.c ${ENTROPY_DIR}huf_decompress.c)

if(BUILD_TOOLS AND NOT (LINK_TOOLS_WITH_LIB AND BUILD_LIBS))
    set(LIZARD_SRCS ${LIZARD_SRCS} ${LIZARD_SRCS_LIB} ${ENTROPY_LIB_SRCS})
endif()

if(BUILD_TOOLS)
    add_executable(lizard ${LIZARD_SRCS})
    install(TARGETS lizard RUNTIME DESTINATION "bin/")
endif()

if(BUILD_LIBS)


    SET(LIBS_TARGETS "")
    IF(WIN32)
        add_library(liblizard STATIC ${LIZARD_SRCS_LIB} ${ENTROPY_LIB_SRCS})
        SET(LIBS_TARGETS liblizard)
    ELSE(WIN32)
        add_library(liblizard SHARED ${LIZARD_SRCS_LIB} ${ENTROPY_LIB_SRCS})
        add_library(liblizard_static STATIC ${LIZARD_SRCS_LIB} ${ENTROPY_LIB_SRCS})
        SET_TARGET_PROPERTIES(liblizard_static PROPERTIES OUTPUT_NAME lizard)
        SET(LIBS_TARGETS liblizard liblizard_static)
    ENDIF(WIN32)

    set_target_properties(liblizard PROPERTIES
    OUTPUT_NAME lizard
    SOVERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}"
    )

    install(TARGETS ${LIBS_TARGETS}
	RUNTIME DESTINATION lib #on Windows: cmake considers dlls as runtime component
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

    install(FILES
    ${LIB_DIR}/lizard_compress.h
    ${LIB_DIR}/lizard_common.h
    ${LIB_DIR}/lizard_frame.h
    DESTINATION include
    )

    set(PREFIX ${CMAKE_INSTALL_PREFIX})
    set(LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
    set(INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
    string(SUBSTRING ${CPACK_PACKAGE_VERSION_PATCH} 1 -1 VERSION)
    configure_file(${LIB_DIR}/liblizard.pc.in liblizard.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liblizard.pc
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig"
    )

  if(BUILD_TOOLS AND LINK_TOOLS_WITH_LIB)
        target_link_libraries(lizard liblizard)
    endif()
endif()


#warnings

if(MSVC)
    ADD_DEFINITIONS("-W4")
endif()
if(GNU_COMPATIBLE_COMPILER)
    ADD_DEFINITIONS("-Wall")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS("-Wextra")
    ADD_DEFINITIONS("-Wundef")
    ADD_DEFINITIONS("-Wshadow")
    ADD_DEFINITIONS("-Wcast-align")
    ADD_DEFINITIONS("-Wstrict-prototypes")
endif(CMAKE_COMPILER_IS_GNUCXX)
if(GNU_COMPATIBLE_COMPILER)
    # we need gnu99 instead of c99 on Linux and Solaris
    # to get C99 and POSIX definitions
    # an alternative with cmake >= 3.1/3.2 is the C_STANDARD property
    ADD_DEFINITIONS("-std=gnu99")
endif()
ADD_DEFINITIONS("-DLIZARD_VERSION=\"${CPACK_PACKAGE_VERSION_PATCH}\"")
INCLUDE_DIRECTORIES (${LIB_DIR})




