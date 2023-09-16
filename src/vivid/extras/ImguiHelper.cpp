#include "ImguiHelper.h"

namespace vivid {

void ImguiHelper::DrawTabHorizontally(const std::string &childName,
                                      const ImVec2 &childSize,
                                      const std::vector<std::string> &tabNames,
                                      int &selectedSubTab) {
    int length = tabNames.front().length(); // get shortest string length
    int strIndex = 1;
    for (int i = 1; i < tabNames.size(); i++) {
        if (length > tabNames.at(i).length()) {
            length = tabNames.at(i).length();
            strIndex = i;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, {0, 0, 0, 0});
    ImGui::BeginChild(childName.c_str(), childSize, true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PopStyleColor();

    int minWidth = ImGui::CalcTextSize(tabNames.at(strIndex).c_str()).x;
    int maxWidth = 200;

    int btnWidth = 32;
    int btnHeight = 32;

    ImVec4 notSelectedTextColor(140.f/255.f, 140.f/255.f, 140.f/255.f, 255.f/255.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5); // round buttons
    for (int i = 0; i < tabNames.size(); i++) {
        std::string it = tabNames.at(i);
        ImGui::PushStyleColor(ImGuiCol_Button, selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_Text, selectedSubTab == i ? ImGui::GetStyle().Colors[ImGuiCol_Text] : notSelectedTextColor);

        if (ImGui::Button(it.c_str(), ImVec2(btnWidth, btnHeight))) selectedSubTab = i;
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
        ImGui::PopStyleColor(2);

    }
    ImGui::PopStyleVar();

    ImGui::EndChild();
}


void ImguiHelper::DrawTabVertically(const std::string &childName,
                                    const ImVec2 &childSize,
                                    const std::vector<std::string> &tabNames,
                                    const std::vector<std::string> &tooltips,
                                    int &selectedSubTab) {
//    ImGui::BeginChild(childName.c_str(), childSize, true);

    ImGui::Spacing();

    ImVec4 col(0, 0, 0, 0);
    ImVec4 buttonActiveColor(239/255.f, 73/255.f, 88/255.f, 1.0f);
    ImVec4 guiTextColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 notSeletecTextColor(140.f/255.f, 140.f/255.f, 140.f/255.f, 255.f/255.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10); // round buttons
    const float button_width = 30;
    for (int i = 0; i < tabNames.size(); i++) {
        std::string it = tabNames[i];
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5));
        ImGui::PushStyleColor(ImGuiCol_Button, selectedSubTab == i ? buttonActiveColor : col);
        ImGui::PushStyleColor(ImGuiCol_Text, selectedSubTab == i ? guiTextColor : notSeletecTextColor);
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - button_width);
        if (ImGui::Button(it.c_str(), ImVec2(button_width, 30))) selectedSubTab = i;
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(tooltips[i].c_str());
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }
    ImGui::PopStyleVar();

//    ImGui::EndChild();
}


} // namespace vivid