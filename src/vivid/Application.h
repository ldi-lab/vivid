#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "vivid/UIManager.h"


namespace vivid {

class Application {
public:
    Application(int windowWidth, int windowHeight, std::string  appName);

    virtual void Run();

    virtual void Update();

    virtual void WindowResizeCallback(int width, int height);

    void SetWindowResizable(bool resizable);

    void SetWindowIcon(unsigned char* data, int width, int height);

    virtual void Render();

    bool ShouldClose();

    void Close();

private:
    void CreateWindow();

protected:
    int windowWidth_;
    int windowHeight_;
    std::string appName_;

    // Window
    GLFWwindow* window_;

    // UI manager
    UIManager ui_;

};

} // namespace vivid