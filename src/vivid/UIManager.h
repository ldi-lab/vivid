#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace vivid {

class UIManager {
public:
    UIManager() = default;

    void Initialize(GLFWwindow* window);

    void NewFrame();

    void Render();

    void Destroy();

public:
    void LoadFonts();

    void LoadTheme();


    // Fonts
    ImFont* bigFont_;

    // Style
    ImGuiStyle* style_;

};

} // namespace vivid
