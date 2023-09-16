project(imgui)
cmake_minimum_required(VERSION 3.8)

set(IMGUI_SRCs
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/implot.cpp
        imgui/implot_items.cpp
)

add_library(imgui ${IMGUI_SRCs})
target_include_directories(imgui PUBLIC ./)
target_link_libraries(imgui glfw)
set_target_properties( imgui PROPERTIES FOLDER "imgui" )