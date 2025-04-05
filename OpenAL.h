#ifndef OPENAL_H
#define OPENAL_H

#include <vector>

namespace OPENAL {

    struct Config {
        int screenWidth;
        int screenHeight;
        const char* windowTitle;
    };

    extern Config config;

    extern void* window;  // Assuming GLFWwindow*, declared as void* here for simplicity
    extern unsigned int vertexShader;

    bool InitWindow(int width, int height, const char* title);
    bool InitGLAD();
    bool InitShaders();

    void CreateSphere(float radius, int numSlices, int numStacks, std::vector<float>& vertices);
    void DrawSphere(float centerX, float centerY, float centerZ, float radius,
                    int numSlices, int numStacks, float r, float g, float b, float a);

    void ClearBackground(float r, float g, float b, float a);

    bool IsKeyPressed(int key);
    bool WindowShouldClose();

    void SwapBuffers();
    void CloseWindow();

    extern const char* vertexShaderSource;
    extern const char* fragmentShaderSource;

}  // namespace OPENAL

#endif //OPENAL_H