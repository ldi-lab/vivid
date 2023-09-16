#include <iostream>
#include <thread>
#include <chrono>
#include "vivid/core/Mesh.h"
#include "vivid/extras/TinyViewer.h"
#include "vivid/utils/IOUtil.h"
#include "vivid/primitives/BoxGeometry.h"
#include "vivid/primitives/AxesHelper.h"

using namespace vivid;

MeshPtr LoadModel(const std::string& filePath) {
    std::vector<float> positions, normals, uvs;
    std::vector<int> indices;
    IOUtil::LoadObj(filePath, positions, normals, uvs);

    std::cout << positions.size()/3 << ", " << normals.size()/3 << ", " << uvs.size()/2 << "\n";

    auto geometry = std::make_shared<Geometry>(&positions, &uvs, &normals);
    auto mesh = std::make_shared<Mesh>(geometry);

    return mesh;

}

int main() {

    std::string leftFootFile = "D:/BlenderProjects/left_foot.obj";
    std::string rightFootFile = "D:/BlenderProjects/rightFoot.obj";
    std::string chestFile = "D:/BlenderProjects/chest.obj";
    std::string rightUpperLegFile = "D:/BlenderProjects/rightUpperLeg.obj";
    auto leftFoot = LoadModel(leftFootFile);
    auto rightFoot = LoadModel(rightFootFile);
    auto chest = LoadModel(chestFile);
    auto rightUpperLeg = LoadModel(rightUpperLegFile);

    TinyViewer tinyViewer("TinyViewer", 1280, 720);
    tinyViewer.AddMesh("leftFoot", leftFoot);
    tinyViewer.AddMesh("rightFoot", rightFoot);
    tinyViewer.AddMesh("chest", chest);
    tinyViewer.AddMesh("rightUpperLeg", rightUpperLeg);

//    auto cubeGeometry = std::make_shared<BoxGeometry>();
//    auto cube = std::make_shared<Mesh>(cubeGeometry);
//    tinyViewer.AddMesh("cube", cube);

    auto axesHelperGeometry = std::make_shared<AxesHelper>();
    auto axesHelper = std::make_shared<Mesh>(axesHelperGeometry, GL_LINES);
    tinyViewer.AddMesh("axesHelper", axesHelper);

    // Logo
    std::string logoFile = "models/apple_logo.png";
    std::string logoName = "PoorVR";
    tinyViewer.SetLogo(logoFile, logoName);

    std::string avatarFile = "models/avatar.png";
    tinyViewer.SetAvatar(avatarFile);

    while (!tinyViewer.ShouldClose()) {

        tinyViewer.Update();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));


    }




    return 0;
}