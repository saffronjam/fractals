cmake_minimum_required(VERSION 3.1.0)
project(Saffron VERSION 0.1.0)

### FLAGS ###
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(THREADS_PREFER_PTHREAD_FLAG ON)

### DIRECTORIES ###
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(RESOURCES_DIR "res")
set(LIBRARY_DIR "lib")
set(INCLUDE_DIR "src")

### EXPORTED DIRECTORIES ###
file(GLOB RESOURCES ${RESOURCES_DIR})
set(SAFFRON_RESOURCES_DIR "${RESOURCES}" PARENT_SCOPE)

### VARIABLES ###
set(PLATFORM_MACRO SE_PLATFORM)

### BUILD TYPE SPECIFIC ###
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(
            -DSE_DEBUG
    )
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
    add_definitions(
            -DSE_RELEASE
    )
elseif (${CMAKE_BUILD_TYPE} MATCHES Dist)
    add_definitions(
            -DSE_DIST
    )
else ()
    message(FATAL_ERROR "Bad configuration selected")
endif ()


### OS SPECIFIC ###
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-D${PLATFORM_MACRO}_WINDOWS)

    file(GLOB SHARED_BINARIES
            "${LIBRARY_DIR}/SFML/bin/*.dll"
            )

    ### EXTERNAL INCLUDES AND LIBRARIES ###
    file(GLOB INCLUDES_EXT
            ${LIBRARY_DIR}/entt/include
            ${LIBRARY_DIR}/imgui/include
            ${LIBRARY_DIR}/SFML/include
            ${LIBRARY_DIR}/spdlog/include
            )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-D${PLATFORM_MACRO}_LINUX)

    find_package(SFML REQUIRED network audio graphics window system)

    file(GLOB INCLUDES_EXT
            ${SFML_INCLUDE_DIR}
            ${LIBRARY_DIR}/entt/include
            ${LIBRARY_DIR}/imgui/include
            ${LIBRARY_DIR}/spdlog/include
            )
else ()
    message(FATAL_ERROR "Unknown OS")
endif ()

### EXPORT ###
set(SAFFRON_INCLUDE_DIRS_EXT ${INCLUDES_EXT} PARENT_SCOPE)
set(SAFFRON_INCLUDE_DIRS_EXT ${INCLUDES_EXT})
set(SAFFRON_SHARED_BINARES ${SHARED_BINARIES} PARENT_SCOPE)

find_package(Threads REQUIRED)


### DEFINITIONS ###
add_definitions(
)

### EXECUTABLE ###
file(GLOB_RECURSE ALL_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
add_library(Saffron "${ALL_SOURCES}")
target_precompile_headers(Saffron PUBLIC "src/SaffronPCH.cpp")

### LOCAL INCLUDES ###
file(GLOB INCLUDES ${INCLUDE_DIR} PARENT_SCOPE)
set(SAFFRON_INCLUDE_DIRS ${INCLUDES})
set(SAFFRON_INCLUDE_DIRS ${INCLUDES} PARENT_SCOPE)
target_include_directories(Saffron PRIVATE ${SAFFRON_INCLUDE_DIRS})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_include_directories(Saffron PUBLIC ${SAFFRON_INCLUDE_DIRS_EXT})
    target_link_directories(Saffron PUBLIC
            ${LIBRARY_DIR}/imgui/lib/windows
            ${LIBRARY_DIR}/SFML/lib
            )
    target_link_libraries(Saffron PUBLIC
            ## SFML
            sfml-graphics-d
            sfml-window-d
            sfml-audio-d
            sfml-network-d
            sfml-system-d
            opengl32
            freetype
            winmm
            gdi32
            openal32
            flac
            vorbisenc
            vorbisfile
            vorbis
            ogg
            ws2_32

            ## imgui
            imgui
            )
else ()
    target_include_directories(Saffron PUBLIC ${SAFFRON_INCLUDE_DIRS_EXT})
    target_link_directories(Saffron PUBLIC
            ${LIBRARY_DIR}/imgui/lib/linux
            )
    target_link_libraries(Saffron PUBLIC
            ${SFML_LIBRARIES}
            Threads::Threads
            imgui
            GL
            )
endif ()

### OUTPUT DIRECTORIES ###
set_target_properties(Saffron PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
        )