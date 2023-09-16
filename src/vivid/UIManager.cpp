#include "UIManager.h"
#include "vivid/Fonts.hpp"

namespace vivid {

void UIManager::Initialize(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Load fonts
    LoadFonts();

    // Load theme
    LoadTheme();
}


void UIManager::Destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}


void UIManager::LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
    io.Fonts->AddFontFromMemoryTTF((void*)poppinsFont, sizeof(poppinsFont), 18, &font_cfg);

    // Icon font
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)fontAwesome, sizeof(fontAwesome), 18, &icons_config, icons_ranges);

    // Big font
    ImFontConfig bigFontCfg;
    bigFontCfg.FontDataOwnedByAtlas = false; // if true it will try to free memory and fail
    bigFont_ = io.Fonts->AddFontFromMemoryTTF((void*)poppinsFont, sizeof(poppinsFont), 24, &bigFontCfg);

}


void UIManager::LoadTheme() {
    style_ = &ImGui::GetStyle();

    // Roundings
    style_->WindowRounding = 6;
    style_->ChildRounding = 6;
    style_->FrameRounding = 2;
    style_->GrabRounding = 2;
    style_->PopupRounding = 2; // Combobox

    // Padding and spacing
    style_->ScrollbarSize = 9;
    style_->FramePadding = ImVec2(6, 3);
    style_->ItemSpacing = ImVec2(4, 4);

    // Colors
    style_->Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 230);
    style_->Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
    style_->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
    style_->Colors[ImGuiCol_ButtonActive] = ImColor(239, 73, 88, 255);
    style_->Colors[ImGuiCol_ButtonHovered] = ImColor(173, 55, 65, 255);
    style_->Colors[ImGuiCol_FrameBg] = ImColor(31, 30, 31, 255);
    style_->Colors[ImGuiCol_FrameBgActive] = ImColor(44, 43, 44, 255);
    style_->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);
    style_->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
    style_->Colors[ImGuiCol_ChildBg] = ImColor(33, 34, 45, 255);
    style_->Colors[ImGuiCol_CheckMark] = ImColor(240, 50, 66, 255);
    style_->Colors[ImGuiCol_SliderGrab] = ImColor(240, 74, 88, 255);
    style_->Colors[ImGuiCol_SliderGrabActive] = ImColor(240, 50, 66, 255);
    style_->Colors[ImGuiCol_Header] = ImColor(240, 50, 66, 255);
    style_->Colors[ImGuiCol_HeaderHovered] = ImColor(240, 74, 88, 255);
    style_->Colors[ImGuiCol_HeaderActive] = ImColor(ImColor(240, 50, 66, 255));
    style_->Colors[ImGuiCol_ResizeGrip] = ImColor(220, 50, 66, 120);
    style_->Colors[ImGuiCol_ResizeGripHovered] = ImColor(250, 50, 66, 140);
    style_->Colors[ImGuiCol_ResizeGripActive] = ImColor(240, 50, 66, 255);
    style_->Colors[ImGuiCol_SeparatorHovered] = ImColor(250, 50, 66, 140);
    style_->Colors[ImGuiCol_SeparatorActive] = ImColor(240, 50, 66, 255);
    style_->Colors[ImGuiCol_TitleBgActive] = ImColor(240, 50, 66, 255);
}


void UIManager::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void UIManager::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


} // namespace vivid
