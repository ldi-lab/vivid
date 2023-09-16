#include <iostream>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Camera.h"
#include "vivid/core/Shader.h"
#include "vivid/OrbitControls.h"
#include "vivid/utils/GlmUtils.h"
#include "vivid/primitives/PlaneGeometry.h"
#include "vivid/primitives/BoxGeometry.h"
#include "vivid/primitives/SphereGeometry.h"
#include "vivid/Fonts.hpp"
#include "vivid/extras/ShaderImpl.h"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION  //necessary for stb_image.h
#include "vivid/utils/stb_image.h"

namespace vivid {
    class ImGuiDemoApp : public Application {
    public:
        ImGuiDemoApp() : Application(960, 720, "cube demo") {
            // Load image
            int imgWidth, imgHeight, imgChannels;
            unsigned char *imgData = stbi_load("models/apple_logo.png", &imgWidth, &imgHeight, &imgChannels, 0);

            std::cout << "img width: " << imgWidth << ", " << imgHeight << ", " << imgChannels << std::endl;

            logoTexture_ = std::make_shared<Texture>(imgData, imgWidth, imgHeight, imgChannels);
        }

        void Render() override {
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // imgui
//            ImGui::StyleColorsDark();
            ui_.NewFrame();

            ImGui::SetNextWindowSize({ 800, 600 });
            ImGui::Begin(" ", 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

            ImGui::Columns(2);
            ImGui::SetColumnOffset(1, 173);

            // Left panel
            RenderLogo();
            ImGui::Spacing();
            RenderTabs();
            RenderUser();

            // Right tabs
            ImGui::NextColumn();
            RenderSubTab0();


            ui_.Render();

        }

        void RenderLogo() {
            ImGui::BeginGroup(); { // group it so we can redirect to Website when its pressed
                ImGui::BeginChild("Logo", ImVec2(158, 50), true);

                ImGui::Image((void*)(intptr_t)logoTexture_->GetHandle(), ImVec2(32, 32));

                ImGui::PushFont(ui_.bigFont_);
                ImGui::SameLine();
                ImGui::SetCursorPosY(11); // dont know how to center it sorry :>
                ImGui::TextUnformatted("Big Paste");
                ImGui::PopFont();
                ImGui::EndChild();

                if (ImGui::IsItemClicked(1)) { // redirect to a website/discord on right click
                    std::cout << "logo clicked!\n";
//                    ::ShellExecuteA(NULL, obf("open").c_str(), obf("https://www.youtube.com/watch?v=dQw4w9WgXcQ").c_str(), NULL, NULL, SW_SHOWDEFAULT);
                }

                ImGui::EndGroup();
            }
        }

        void RenderTabs() {
            ImGui::BeginChild("tabs", ImVec2(158, 220), true);

//            ImGuiTextFilter2 filter;
//            filter.Draw2(obf(ICON_FA_SEARCH" Search").c_str(), 140);
            ImGui::Spacing();

            ImVec4 col(0, 0, 0, 0);
            ImVec4 notSeletecTextColor(140.f/255.f, 140.f/255.f, 140.f/255.f, 255.f/255.f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10); // round buttons
            std::vector<std::string> tabNames = {
                    ICON_FA_CROSSHAIRS  " LegitBot",
                    ICON_FA_EYE " Visuals",
                    ICON_FA_COG " Misc",
                    ICON_FA_SAVE " Configs"
            };
            static int selectedTab = 0;
            for (int i = 0; i < tabNames.size(); i++) {
                std::string it = tabNames[i];
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5));
                ImGui::PushStyleColor(ImGuiCol_Button, selectedTab == i ? ui_.style_->Colors[ImGuiCol_ButtonActive] : col);
                ImGui::PushStyleColor(ImGuiCol_Text, selectedTab == i ? ui_.style_->Colors[ImGuiCol_Text] : notSeletecTextColor);
                if (ImGui::Button(it.c_str(), ImVec2(140, 40))) selectedTab = i;
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);
            }
            ImGui::PopStyleVar();

            ImGui::EndChild();
        }

        void RenderUser() {

            int height = 80;
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - height - ui_.style_->ItemSpacing.y));
            ImGui::BeginChild("User", ImVec2(158, height), true);

            ImGui::EndChild();
        }

        void RenderSubTab0()  {
            std::vector<std::string> arr = { "AimAssist", "TriggerBot", "Other" };
            DrawTabHorizontally("subtab-0", ImVec2(ImGui::GetContentRegionAvail().x, 50), arr, selectedSubTab0_);
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
            ImGui::BeginChild("modules-wrapper", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
            ImGui::PopStyleColor();

            switch (selectedSubTab0_) {
                case 0: {
                    ImGui::Columns(2, nullptr, false);
                    ImGui::SetColumnOffset(1, 300);

                    ImGui::BeginChild("aimassist", ImVec2(ImGui::GetContentRegionAvail().x, 300), true);
//                    AimAssist::i().renderImGui();
                    ImGui::EndChild();

                    ImGui::Spacing();

                    ImGui::BeginChild("aimassist2", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
//                    RCS::i().renderImGui();
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("aimassist3", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
                    ImGui::EndChild();

                    break;
                }
                case 1: {

                    break;
                }
                case 2: {

                    break;
                }
            }

            ImGui::EndChild();
        }

        void DrawTabHorizontally(std::string childName, ImVec2 childSize, std::vector<std::string>tabNames, int& selectedSubTab) {
            int length = tabNames.front().length(); // get shortest string length
            int strIndex = 1;
            for (int i = 1; i < tabNames.size(); i++) {
                if (length > tabNames.at(i).length()) {
                    length = tabNames.at(i).length();
                    strIndex = i;
                }
            }

            ImGui::BeginChild(childName.c_str(), childSize, true, ImGuiWindowFlags_HorizontalScrollbar);

            int minWidth = ImGui::CalcTextSize(tabNames.at(strIndex).c_str()).x;
            int maxWidth = 200;

            int btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * (tabNames.size())) / tabNames.size();
            int btnHeight = std::min(std::max((int)ImGui::GetContentRegionAvail().y, 20), 60);
            btnWidth = (std::max)(minWidth, (std::min)(btnWidth, maxWidth));

            { // center buttons
                // tell Dear ImGui to render the button at the new pos
                ImGui::SetCursorPosX((childSize.x - btnWidth * tabNames.size() - ImGui::GetStyle().ItemSpacing.x) / 2);
            }

            ImVec4 notSelectedTextColor(140.f/255.f, 140.f/255.f, 140.f/255.f, 255.f/255.f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5); // round buttons
            for (int i = 0; i < tabNames.size(); i++) {
                std::string it = tabNames.at(i);
                ImGui::PushStyleColor(ImGuiCol_Button, selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
                ImGui::PushStyleColor(ImGuiCol_Text, selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_Text] : notSelectedTextColor);


                if (ImGui::Button(it.c_str(), ImVec2(btnWidth, btnHeight))) selectedSubTab = i;
                ImGui::SameLine();
                ImGui::PopStyleColor(2);

            }
            ImGui::PopStyleVar();

            ImGui::EndChild();
        }

    private:

        std::shared_ptr<Texture> logoTexture_;

        int selectedSubTab0_ = 0;

    };
} // namespace vivid


int main() {
    vivid::ImGuiDemoApp app;
    app.Run();

    return 0;
}