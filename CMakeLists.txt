cmake_minimum_required(VERSION 3.1.0)
project(Fractals VERSION 0.1.0)

### DIRECTORIES ###
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(RESOURCES_DIR "res")
set(LIBRARIES_DIR "lib")
set(SUBDIRECTORIES_DIR "deps")
set(INCLUDES_DIR "src")

### Saffron Engine ###
add_subdirectory("${SUBDIRECTORIES_DIR}/Saffron")

### FLAGS ###
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(THREADS_PREFER_PTHREAD_FLAG ON)

### VARIABLES ###
set(PLATFORM_MACRO SE_PLATFORM)

### COMPILER SPECIFIC ###
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(COMPILER_LIB_DIR "msvc")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/w44244)
    add_compile_options(/w44267)

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(COMPILER_LIB_DIR "mingw")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=nehalem")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1 -msse4.2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
else ()
    message(FATAL_ERROR "Bad compiler selected. Supported is GNU and MSVC")
endif ()

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

    file(COPY ${SAFFRON_SHARED_BINARES} DESTINATION ${BUILD_DIR})

else ()
    add_definitions(-D${PLATFORM_MACRO}_LINUX)

endif ()

file(COPY ${SAFFRON_RESOURCES_DIR} DESTINATION ${BUILD_DIR})
file(COPY ${RESOURCES_DIR} DESTINATION ${BUILD_DIR})
find_package(Threads REQUIRED)

### DEFINITIONS ###
add_definitions()

### EXECUTABLE ###
file(GLOB_RECURSE ALL_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
set(ClientSRC "${ALL_SOURCES}")
add_executable(Client "${ClientSRC}")
target_precompile_headers(Saffron PUBLIC "deps/Saffron/src/SaffronPCH.cpp")

### INCLUDES ###
target_include_directories(Client PRIVATE
        ${INCLUDES_DIR}
        ${SAFFRON_INCLUDE_DIRS}
        ${SAFFRON_INCLUDE_DIRS_EXT}
        )

### LIBRARIES ###
target_link_libraries(Client PRIVATE Saffron)

### OUTPUT DIRECTORIES ###
set_target_properties(Client PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
        )
