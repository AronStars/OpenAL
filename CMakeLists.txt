cmake_minimum_required(VERSION 3.30)
project(OpenAL)

include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# Fetch glad
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# Fetch glm (header-only)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

set(CMAKE_CXX_STANDARD 26)

# Declare your library FIRST
add_library(OpenAL STATIC OpenAL.cpp)

# Link libraries AFTER creating your build target
target_link_libraries(OpenAL
        PRIVATE
        glm
        glfw
        glad
)