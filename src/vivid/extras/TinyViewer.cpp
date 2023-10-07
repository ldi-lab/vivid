#include "vivid/extras/TinyViewer.h"
#include "vivid/extras/ImguiHelper.h"
#include "vivid/Fonts.hpp"
#define STB_IMAGE_IMPLEMENTATION  //necessary for stb_image.h
#include "vivid/utils/stb_image.h"


namespace vivid {

TinyViewer::TinyViewer(const std::string &appName, int windowWidth, int windowHeight)
        : Application(windowWidth, windowHeight, appName)
{
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    meshShader_ = std::make_shared<Shader>("BlinnPhongColored.vert", "BlinnPhongColored.frag");

    // Camera
    mainCam_ = std::make_shared<Camera>();
    mainCam_->GetTransform().SetPosition(Eigen::Vector3d(2, 0, 0));
    mainCam_->LookAt(Eigen::Vector3d(0, 0, 0), {0, 0, 1});

    // Orbit controls
    orbitControl_ = std::make_shared<OrbitControls>(window_, mainCam_);

}


void TinyViewer::Render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    orbitControl_->Update();

    // Bind shader
    meshShader_->Use();

    // set uniforms0
    meshShader_->SetVec4("light.positionC", glm::vec4(1, 2, 5, 6));
    meshShader_->SetVec3("light.color", glm::vec3(1.0, 1.0, 1.0));
    meshShader_->SetFloat("light.intensity", 1);

    meshShader_->SetVec3("material.diffuseColor", glm::vec3(0.7, 0.7, 0.7));
    meshShader_->SetVec3("material.specularColor", glm::vec3(1, 0, 0));
    meshShader_->SetFloat("material.shininess", 5);

    meshShader_->SetVec3("envAmbientColor", glm::vec3(0.2, 0.2, 0.2));

    // Draw mesh
    for (auto &it : meshes_) {
        it.second->Draw(mainCam_, meshShader_);
    }

    RenderUI();

}


void TinyViewer::AddMesh(const std::string &name, vivid::MeshPtr mesh) {
    meshes_[name] = mesh;
}


void TinyViewer::SetLogo(const std::string &logoImgFile, const std::string &logoName) {
    int imgWidth, imgHeight, imgChannels;
    unsigned char* imgData = stbi_load(logoImgFile.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
    logoTexture_ = std::make_shared<Texture>(imgData, imgWidth, imgHeight, imgChannels);
    logoName_ = logoName;
}


void TinyViewer::SetAvatar(const std::string &avatarImgFile) {
    int imgWidth, imgHeight, imgChannels;
    unsigned char* imgData = stbi_load(avatarImgFile.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
    avatarTexture_ = std::make_shared<Texture>(imgData, imgWidth, imgHeight, imgChannels);
}



void TinyViewer::RenderUI() {
    ui_.NewFrame();

    if (logoTexture_ != nullptr) {
        RenderLogo();
    }

    if (selectedMenuTab_ == -1) {
        RenderMenus();
    } else {
        RenderRightPanel();
    }


//    ImGui::SetNextWindowSize({})



    ui_.Render();
}


void TinyViewer::RenderRightPanel() {
    const int panelWidth = 360;
    const int panelHeight = windowHeight_;
    ImGui::SetNextWindowSize({panelWidth, (float)panelHeight});
    ImGui::SetNextWindowPos({(float)windowWidth_ - panelWidth, 0});

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("RightPanel", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize
                                        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({panelWidth-50, ImGui::GetCursorPos().y+10});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12);
    if(ImGui::Button(">", ImVec2(24,24)) ) {
        selectedMenuTab_ = -1;
    }
    ImGui::PopStyleVar();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY()-30);
    std::vector<std::string> tabNames = {ICON_FA_CROSSHAIRS,
                                         ICON_FA_EYE ,
                                         ICON_FA_WIFI,
                                         ICON_FA_BOOK};
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+18);
    ImguiHelper::DrawTabHorizontally("tabs", {ImGui::GetContentRegionAvail().x-60, 50}, tabNames, selectedMenuTab_);
    ImGui::Separator();


    if (selectedMenuTab_ == 0) {
        RenderAvatar();
    }
    else if (selectedMenuTab_ == 1) {
        RenderTrackerInfo();
    }
    else {
        ImGui::Text("Hello, world!");
    }

    ImGui::End();
}


void TinyViewer::RenderAvatar() {
    float panelWidth = ImGui::GetContentRegionAvail().x;

    ImGui::Spacing();
    const std::string avatarTitle = "ComboVR Suit";
    ImGui::SetCursorPosX(panelWidth/2 - ImGui::CalcTextSize(avatarTitle.c_str()).x/2);
    ImGui::Text(avatarTitle.c_str());

    ImGui::Spacing();

    float imgWidth = 180;
    ImGui::SetCursorPosX(panelWidth/2 - imgWidth/2);
    ImGui::Image((void*)(intptr_t)avatarTexture_->GetHandle(), ImVec2(imgWidth, imgWidth*1.62));
    ImGui::Separator();
}


void TinyViewer::RenderTrackerInfo() {
    for (int i = 0; i < 5; i++) {
        ImGui::BeginChild(("hip tracker" + std::to_string(i)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 45), true, ImGuiWindowFlags_HorizontalScrollbar); {
            ImGui::Image((void*)(intptr_t)logoTexture_->GetHandle(), ImVec2(25, 25));
        ImGui::SameLine();
        ImGui::Text("Hip");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+10);
        ImGui::Text(ICON_FA_WIFI " Active");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+10);
        ImGui::Text(ICON_FA_BATTERY_FULL " 100");
            ImGui::EndChild();
            ImGui::Spacing();
        }

    }

}


void TinyViewer::RenderLogo() {
    ImGui::SetNextWindowSize({180, 80});
    ImGui::SetNextWindowPos({0, 0});

//    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Logo", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize
                            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
//    ImGui::PopStyleColor();

    ImGui::BeginGroup(); { // group it so we can redirect to Website when its pressed
//        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0.2));
        ImGui::BeginChild("Logo", ImVec2(158, 50), true);
//        ImGui::PopStyleColor();

        ImGui::Image((void*)(intptr_t)logoTexture_->GetHandle(), ImVec2(32, 32));

        ImGui::PushFont(ui_.bigFont_);
        ImGui::SameLine();
        ImGui::SetCursorPosY(11); // don't know how to center it sorry :>
        ImGui::TextUnformatted(logoName_.c_str());
        ImGui::PopFont();
        ImGui::EndChild();

        if (ImGui::IsItemClicked(1)) { // redirect to a website/discord on right click
            std::cout << "logo clicked!\n";
//                    ::ShellExecuteA(NULL, obf("open").c_str(), obf("https://www.youtube.com/watch?v=dQw4w9WgXcQ").c_str(), NULL, NULL, SW_SHOWDEFAULT);
        }

        ImGui::EndGroup();
    }

    ImGui::End();
}


void TinyViewer::RenderMenus() {
    const std::vector<std::string> menus = {
            ICON_FA_CROSSHAIRS,
            ICON_FA_EYE ,
            ICON_FA_WIFI,
            ICON_FA_BOOK
    };
    const std::vector<std::string> tooptips = {
            "calibrate", "eye", "settings", "docs"
    };

    const float menuWidth = 120;
    ImGui::SetNextWindowSize({menuWidth, 400});
    ImGui::SetNextWindowPos({windowWidth_-menuWidth, 100});

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Menu", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize
                            | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleColor();

    int selectedTab = -1;
    ImguiHelper::DrawTabVertically("menu", {220, 360}, menus, tooptips, selectedTab);
    if (selectedTab >= 0) {
        selectedMenuTab_ = selectedTab;
    }


    ImGui::End();

}


} // namespace vivid-