file(GLOB_RECURSE VIVID_SRCS
        vivid/*.hpp
        vivid/*.cpp
        vivid/*.h
        vivid/*.cc
)

add_library(vivid ${VIVID_SRCS})
target_include_directories(vivid PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GLM_INCLUDE_DIRS}
        )
target_link_libraries(vivid PUBLIC
        glfw glad imgui glm
)