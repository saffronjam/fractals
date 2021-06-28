cmake_minimum_required(VERSION 3.1.0)
project(Algorithms VERSION 0.1.0)

### DIRECTORIES ###
set(BuildDir "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(ResourceDir "res")
set(LibrariesDir "lib")
set(SubDirectoriesDir "deps")
set(IncludeDir "src")


### FLAGS ###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(THREADS_PREFER_PTHREAD_FLAG ON)

### VARIABLES ###
set(PLATFORM_MACRO SE_PLATFORM)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(BuildType Debug)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
    set(BuildType Release)
elseif (${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
    set(BuildType Dist)
else ()
    message(FATAL_ERROR "Bad configuration selected")
endif ()

### COMPILER SPECIFIC ###
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CompilerLibDir msvc)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/w44244)
    add_compile_options(/w44267)

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CompilerLibDir mingw)
else ()
    message(FATAL_ERROR "Bad compiler selected. Supported is GNU and MSVC")
endif ()

### OS SPECIFIC ###
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(Platform windows)
    add_definitions(-D${PLATFORM_MACRO}_WINDOWS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(Platform Linux)
    add_definitions(-D${PLATFORM_MACRO}_LINUX)
else ()
    message(FATAL_ERROR "Unknown OS. Only Windows and Linux are supported.")
endif ()

### BUILD TYPE SPECIFIC ###
if (BuildType STREQUAL Debug)
    set(OutputDir ${BuildDir})
    add_definitions(-DSE_IMGUI_INI_PATH="${PROJECT_SOURCE_DIR}/imgui.ini")
    add_definitions(
            -DSE_DEBUG
    )
elseif (BuildType STREQUAL Release)
    set(OutputDir ${BuildDir})
    add_definitions(-DSE_IMGUI_INI_PATH="${PROJECT_SOURCE_DIR}/imgui.ini")
    add_definitions(
            -DSE_RELEASE
    )
elseif (BuildType STREQUAL Dist)
    set(OutputDir "${PROJECT_SOURCE_DIR}/example/${Platform}")

    # Copy ini-file to dist output
    set(IniPath "${PROJECT_SOURCE_DIR}/imgui.ini")
    if(EXISTS ${IniPath})
        file(COPY "${PROJECT_SOURCE_DIR}/imgui.ini" DESTINATION ${OutputDir})
    endif()
    add_definitions(-DSE_IMGUI_INI_PATH="${IniPath}")

    add_definitions(
            -DSE_DIST
    )
endif ()

## Connect to subdirectory
add_subdirectory("${SubDirectoriesDir}/SaffronEngine2D")

if (Platform STREQUAL windows)
    file(COPY ${SaffronSharedBinaries} DESTINATION ${OutputDir})
endif ()

file(COPY ${SaffronResourceDir} DESTINATION ${OutputDir})
file(COPY ${ResourceDir} DESTINATION ${OutputDir})
find_package(Threads REQUIRED)


### EXECUTABLE ###
file(GLOB_RECURSE allSources "${CMAKE_SOURCE_DIR}/*.cpp")
set(ClientSrc "${allSources}")
add_executable(Client "${ClientSrc}")
target_precompile_headers(Saffron PUBLIC "deps/SaffronEngine2D/src/SaffronPCH.cpp")

### INCLUDES ###
target_include_directories(Client PRIVATE
        ${IncludeDir}
        ${SaffronIncludeDirs}
        ${SaffronIncludeDirsExt}
        )

message(hello ${SaffronIncludeDirsExt})

### LIBRARIES ###
target_link_libraries(Client PRIVATE Saffron)

### OUTPUT DIRECTORIES ###
set_target_properties(Client PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${OutputDir}
        LIBRARY_OUTPUT_DIRECTORY ${OutputDir}
        RUNTIME_OUTPUT_DIRECTORY ${OutputDir}
        )