#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cmake-build-debug/_deps/glfw-src/deps/glad/gl.h"

//  OPENAL - A simplified graphics library for drawing spheres (and more, potentially)
//  -----------------------------------------------------------------------------
//  This library provides a basic set of functions for creating a window,
//  drawing a sphere, and handling basic program setup. It's built using OpenGL.
//  It is named OPENAL as requested by the user. It is NOT related to OpenAL.

namespace OPENAL {

    // Configuration
    struct Config {
        int screenWidth;
        int screenHeight;
        const char *windowTitle;
    };

    // Window and OpenGL context
    GLFWwindow *window = nullptr;
    GLuint vertexShader, fragmentShader, shaderProgram;

    // Function to initialize GLFW and create a window
    bool InitWindow(int width, int height, const char *title) {
        if (!glfwInit()) {
            std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        });
        return true;
    }

    // Function to initialize GLAD
    bool InitGLAD() {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
            return false;
        }
        return true;
    }

    // Vertex Shader Source Code (for 3D)
    const char *vertexShaderSource = R"(
        #version 330 core
        in vec3 aPos;
        uniform mat4 uModelViewProjection;
        void main() {
            gl_Position = uModelViewProjection * vec4(aPos, 1.0);
        }
    )";

    // Fragment Shader Source Code (for 3D)
    const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 uColor;
        void main() {
            FragColor = uColor;
        }
    )";

    // Function to compile and link shaders
    bool InitShaders() {
        // Vertex Shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Vertex shader compilation failed\n" << infoLog << std::endl;
            return false;
        }

        // Fragment Shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Fragment shader compilation failed\n" << infoLog << std::endl;
            return false;
        }

        // Shader Program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return true;
    }

    // Function to create the vertices for a sphere
    void CreateSphere(float radius, int numSlices, int numStacks, std::vector<float> &vertices) {
        for (int i = 0; i <= numStacks; ++i) {
            float phi = i * M_PI / numStacks;
            for (int j = 0; j <= numSlices; ++j) {
                float theta = j * 2 * M_PI / numSlices;
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * cosf(phi);
                float z = radius * sinf(phi) * sinf(theta);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
    }

    // Function to draw a sphere
    void DrawSphere(float centerX, float centerY, float centerZ, float radius, int numSlices, int numStacks, float r, float g, float b, float a) {
        if (!window) return;

        std::vector<float> vertices;
        CreateSphere(radius, numSlices, numStacks, vertices);

        std::vector<unsigned int> indices;
        for (int i = 0; i < numStacks; ++i) {
            for (int j = 0; j < numSlices; ++j) {
                int row1 = i * (numSlices + 1);
                int row2 = (i + 1) * (numSlices + 1);

                indices.push_back(row1 + j);
                indices.push_back(row1 + j + 1);
                indices.push_back(row2 + j);

                indices.push_back(row2 + j);
                indices.push_back(row1 + j + 1);
                indices.push_back(row2 + j + 1);
            }
        }

        GLuint VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glUseProgram(shaderProgram);
        // Set the color uniform
        int colorLocation = glGetUniformLocation(shaderProgram, "uColor");
        glUniform4f(colorLocation, r, g, b, a);

        // Model, View, Projection matrices
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(centerX, centerY, centerZ));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)Config::screenWidth / (float)Config::screenHeight, 0.1f, 100.0f);

        glm::mat4 modelViewProjection = projection * view * model;
        int mvpLocation = glGetUniformLocation(shaderProgram, "uModelViewProjection");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

    }

    // Function to clear the background
    void ClearBackground(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Also clear the depth buffer
    }

     // Function to get if a key is pressed.
    bool IsKeyPressed(int key)
    {
       return glfwGetKey(window, key) == GLFW_PRESS;
    }

    // Function to check if the window should close
    bool WindowShouldClose() {
        if (!window) return true;
        return glfwWindowShouldClose(window);
    }

    // Function to swap buffers and poll events
    void SwapBuffers() {
        if (window) {
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // Function to terminate GLFW
    void CloseWindow() {
        if (window) {
            glfwDestroyWindow(window);
            glfwTerminate();
            window = nullptr;
        }
    }

    // Global configuration
    Config config;

} // namespace OPENAL

int main() {
    // Configuration
    OPENAL::config.screenWidth = 800;
    OPENAL::config.screenHeight = 600;
    OPENAL::config.windowTitle = "OPENAL Example - Sphere";

    // Initialize the window
    if (!OPENAL::InitWindow(OPENAL::config.screenWidth, OPENAL::config.screenHeight, OPENAL::config.windowTitle)) {
        return -1;
    }

    // Initialize GLAD
    if (!OPENAL::InitGLAD()) {
        return -1;
    }

    // Initialize Shaders
    if (!OPENAL::InitShaders()) {
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, OPENAL::config.screenWidth, OPENAL::config.screenHeight);


    // Main loop
    while (!OPENAL::WindowShouldClose()) {
        // Clear the background and the depth buffer
        OPENAL::ClearBackground(0.2f, 0.3f, 0.3f, 1.0f);

        // Draw a sphere
        float sphereX = 0.0f;
        float sphereY = 0.0f;
        float sphereZ = 0.0f;
        float sphereRadius = 1.0f;
        int numSlices = 30;
        int numStacks = 30;
        OPENAL::DrawSphere(sphereX, sphereY, sphereZ, sphereRadius, numSlices, numStacks, 1.0f, 0.0f, 0.0f, 1.0f);

       OPENAL::SwapBuffers();
       if (OPENAL::IsKeyPressed(GLFW_KEY_ESCAPE))
           glfwSetWindowShouldClose(OPENAL::window, true);
    }

    // Close the window and terminate GLFW
    OPENAL::CloseWindow();

    return 0;
}
