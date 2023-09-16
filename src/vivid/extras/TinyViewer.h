#pragma once

#include <iostream>
#include <map>
#include "vivid/Application.h"
#include "vivid/core/Mesh.h"
#include "vivid/core/Texture.h"
#include "vivid/extras/FrameBuffer.h"
#include "vivid/OrbitControls.h"

namespace vivid {

class TinyViewer : public Application {
public:
    TinyViewer(const std::string &appName, int windowWidth = 960, int windowHeight = 720);

    virtual void Render() override;

    void AddMesh(const std::string& name, MeshPtr mesh);

    void SetLogo(const std::string& logoImgFile, const std::string& logoName);

    void SetAvatar(const std::string& avatarImgFile);

    void RenderUI();

    void RenderLogo();

    void RenderMenus();

    void RenderRightPanel();

    void RenderAvatar();

    void RenderTrackerInfo();

private:
    std::map<std::string, std::shared_ptr<vivid::Mesh> > meshes_;

    std::shared_ptr<vivid::Mesh> groundPlane_;

    // Directional light
    vivid::CameraPtr lightCam_;

    // Camera
    vivid::CameraPtr mainCam_;

    // shaders
    std::shared_ptr<vivid::Shader> meshShader_;
    std::shared_ptr<vivid::Shader> shadowShader_;
    std::shared_ptr<vivid::Shader> groundShader_;

    // frame buffer for shadow
    std::shared_ptr<vivid::FrameBuffer> frameBuffer_;

    // Orbit control
    std::shared_ptr<vivid::OrbitControls> orbitControl_;

    // Logo
    std::shared_ptr<Texture> logoTexture_ = nullptr;
    std::string logoName_;

    //  Avatar
    std::shared_ptr<Texture> avatarTexture_ = nullptr;

    int selectedMenuTab_ = -1;


};


} // namespace vivid
