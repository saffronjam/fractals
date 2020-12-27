cmake_minimum_required(VERSION 3.1.0)
project(ImGui)

file(GLOB INCLUDES "${PROJECT_SOURCE_DIR}")
file(GLOB SRC_FILES
        "imconfig.h"
        "imgui.h"
        "imgui.cpp"
        "imgui_draw.cpp"
        "imgui_internal.h"
        "imgui_widgets.cpp"
        "imstb_rectpack.h"
        "imstb_textedit.h"
        "imstb_truetype.h"
        "imgui_demo.cpp"
)

add_library(ImGui ${SRC_FILES})

target_include_directories(ImGui PRIVATE "${INCLUDES}")
set (ImGui_INCLUDES "${INCLUDES}" PARENT_SCOPE)