#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <EGL/program.h>
#include <EGL/shader.h>
#include <EGL/buffer.h>
#include <EGL/debug.h>
#include <EGL/windowContext.h>

class TestWindow : public egl::WindowContext {
public:
    TestWindow(int width, int height, const char* name) : egl::WindowContext(width, height, name) {}

    void run() override {
        useContext();

        std::vector<float> positions = {
            -0.5f, -0.5f,
             0.0f,  0.5f,
             0.5f, -0.5f,

             0.5f, -0.5f,
             0.0f,  0.5f,
             1.0f,  0.5f
        };

        egl::Buffer vbo(egl::BufferType::Array);
        vbo.setData(positions, egl::BufferUsage::StaticDraw);
        vbo.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0); // define attribute 0 of the vbo

        egl::Shader vertex(egl::ShaderType::Vertex, std::ifstream("../../res/shaders/basicTriangle/vertex.shader"));
        egl::Shader fragment(egl::ShaderType::Fragment, std::ifstream("../../res/shaders/basicTriangle/fragment.shader"));
        egl::Program program;

        program.attach(vertex);
        program.attach(fragment);

        program.link();

        program.bind();

        while (!shouldClose())
        {
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

            GL_CALL(glDrawArrays(GL_TRIANGLES, 0, (int)(positions.size() / 2)));

            swapBuffers();

            egl::pollEvents();
        }
    }
};

int main(void)
{
    TestWindow window(640, 480, "Test Window");
    window.run();

    return 0;
}