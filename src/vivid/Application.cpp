#include "Application.h"
#include "Fonts.hpp"
#include <utility>
#include <functional>


namespace vivid {

Application::Application(int windowWidth, int windowHeight, std::string  appName)
    : windowWidth_(windowWidth), windowHeight_(windowHeight), appName_(std::move(appName))
{
    CreateWindow();
    // Initialize GLAD before we call any OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "failed to initialize GLAD\n";
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);

    // This is necessary if we want to change point size when rendering in GL_POINTS mode.
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_MULTISAMPLE);   // enable multi-sampling
}


void Application::CreateWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // tell glfw to create buffers for multi-sampling, this is for MSAA(multisample anti-aliasing)
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(windowWidth_, windowHeight_, appName_.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        std::cerr << "failed to create GLFW window!\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);    // Set user pointer to this application instance

    // Initialize ImGui
    ui_.Initialize(window_);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->WindowResizeCallback(width, height);
    });

}


void Application::Run() {

    std::cout << "Run " << appName_ << "\n";
    while (!glfwWindowShouldClose(window_)) {
        Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    glfwTerminate();
    std::cout << "Terminate " << appName_ << "\n";
}


void Application::Update() {
    Render();
    glfwPollEvents();
    glfwSwapBuffers(window_);
}


void Application::Render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void Application::WindowResizeCallback(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void Application::SetWindowResizable(bool resizable) {
    glfwSetWindowAttrib(window_, GLFW_RESIZABLE, resizable);
}


bool Application::ShouldClose() {
    return glfwWindowShouldClose(window_);
}


void Application::Close() {
    ui_.Destroy();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

} // namespace vivid