#pragma once

#include <iostream>
#include <vector>
#include "imgui/imgui.h"

namespace vivid {

class ImguiHelper {
public:
    ImguiHelper() = default;

    static void DrawTabHorizontally(const std::string &childName,
                                         const ImVec2 &childSize,
                                         const std::vector<std::string> &tabNames,
                                         int& selectedSubTab);


    static void DrawTabVertically(const std::string &childName,
                                  const ImVec2 &childSize,
                                  const std::vector<std::string> &tabNames,
                                  const std::vector<std::string> &tooltips,
                                  int& selectedSubTab);


};

} // namespace vivid
