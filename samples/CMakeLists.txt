link_libraries(vivid glfw glad)

add_executable(HelloGL basic/HelloGL.cpp)

add_executable(CubeDemo basic/CubeDemo.cpp)

add_executable(ScreenShaderDemo basic/ScreenShaderDemo.cpp)

add_executable(LoadJsonDemo basic/LoadJsonDemo.cpp)

add_executable(PrimitivesDemo basic/PrimitivesDemo.cpp)

add_executable(ImGuiDemo basic/ImGuiDemo.cpp)

add_executable(ColoredBlinnPhongDemo basic/ColoredBlinnPhongDemo.cpp)

add_executable(FrameBufferDemo basic/FrameBufferDemo.cpp)

add_executable(ShadowMapDemo basic/ShadowMapDemo.cpp)

file(COPY ../assets/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../assets/models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})