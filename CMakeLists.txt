cmake_minimum_required(VERSION 3.28)
get_filename_component(ROOT_DIR_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${ROOT_DIR_NAME} LANGUAGES C VERSION 0.0.1)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create and run from a build/ directory.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(LIB_NAME Papyros)
string(TOLOWER "${LIB_NAME}" LIB_NAME_LOWER)

# Source file caching
set(SRC_FILES_CACHE "${CMAKE_BINARY_DIR}/src_files.cache")
if(EXISTS ${SRC_FILES_CACHE})
    file(READ ${SRC_FILES_CACHE} CACHED_SRC_FILES)
    string(REGEX REPLACE "\n" ";" CACHED_SRC_FILES "${CACHED_SRC_FILES}")
else()
    file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        src/*.c
        external/*.c
    )
    file(WRITE ${SRC_FILES_CACHE} "${SRC_FILES}")
    set(CACHED_SRC_FILES "${SRC_FILES}")
endif()
set(SRC_FILES "${CACHED_SRC_FILES}")

# Static library
add_library(${LIB_NAME}_static STATIC ${SRC_FILES})
# Dynamic library
add_library(${LIB_NAME}_shared SHARED ${SRC_FILES})

target_include_directories(${LIB_NAME}_static PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/external
)
target_include_directories(${LIB_NAME}_shared PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/external
)

set(DEBUG_FLAGS -Wall -Wextra -Werror -MMD -MF -pedantic -ggdb -DDEBUG)
set(RELEASE_FLAGS -Wall -Wextra -Werror -O2 -DNDEBUG)
target_compile_options(${LIB_NAME}_static PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
    $<$<CONFIG:Release>:${RELEASE_FLAGS}>
)
target_compile_options(${LIB_NAME}_shared PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
    $<$<CONFIG:Release>:${RELEASE_FLAGS}>
)

if(DEFINED CMAKE_FLAGS_EXTRA)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_FLAGS_EXTRA}")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
set(OUTPUT_LIB_DIR "${CMAKE_BINARY_DIR}/lib/${BUILD_TYPE_LOWER}")
foreach(TARGET ${LIB_NAME}_static ${LIB_NAME}_shared)
    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR}
        OUTPUT_NAME ${LIB_NAME_LOWER}
    )
endforeach()

# Write the library paths to .pounce/data/library_path.pounce
set(LIBRARY_PATH_FILE "${CMAKE_SOURCE_DIR}/.pounce/data/library_path.pounce")
add_custom_command(TARGET ${LIB_NAME}_static POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${LIB_NAME}_static>"
    COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:${LIB_NAME}_static>" > "${LIBRARY_PATH_FILE}"
    COMMENT "Writing static library path to ${LIBRARY_PATH_FILE}"
)
add_custom_command(TARGET ${LIB_NAME}_shared POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${LIB_NAME}_shared>"
    COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:${LIB_NAME}_shared>" >> "${LIBRARY_PATH_FILE}"
    COMMENT "Appending shared library path to ${LIBRARY_PATH_FILE}"
)

message(STATUS "-----------------------------------------------")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "C Flags: ${DEBUG_FLAGS}")
    message(STATUS "Extra Flags: ${CMAKE_FLAGS_EXTRA}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "C Flags: ${RELEASE_FLAGS}")
    message(STATUS "Extra Flags: ${CMAKE_FLAGS_EXTRA}")
else()
    message(STATUS "C Flags: (unknown build type)")
endif()
message(STATUS "Sources: ${SRC_FILES}")
message(STATUS "Output: ${OUTPUT_LIB_DIR}/lib${LIB_NAME_LOWER}.{a,so}")
message(STATUS "Library Path File: ${LIBRARY_PATH_FILE}")
message(STATUS "-----------------------------------------------")
file(WRITE "${CMAKE_SOURCE_DIR}/.pounce/data/target_type" "library")

