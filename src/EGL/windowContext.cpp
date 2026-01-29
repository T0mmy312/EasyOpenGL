#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <EGL/windowContext.h>

#include <mutex>
#include <stdexcept>
#include <iostream>

namespace egl {

namespace {
    int glfwRefCount = 0;
    bool glfwInitialized = false;
}

void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW error (" << error << "): " << description << std::endl;
}

bool initGLFW() {
    if (!glfwInitialized) {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            return false;
        glfwInitialized = true;
    }

    ++glfwRefCount;
    return true;
}

void terminateGLFW() {
     if (!glfwInitialized)
        return;

    --glfwRefCount;

    if (glfwRefCount == 0) {
        glfwTerminate();
        glfwInitialized = false;
    }
}

void pollEvents() {
    glfwPollEvents();
}

// ----------------------------------------------------------------------------------------------------
// class Program
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// protected methods
// --------------------------------------------------

void WindowContext::useContext() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    glfwMakeContextCurrent(window);
}

bool WindowContext::shouldClose() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    return glfwWindowShouldClose(window);
}

void WindowContext::swapBuffers() {
    if (window == NULL)
        throw std::runtime_error("GLFW Window is invalid!");
    glfwSwapBuffers(window);
}

// --------------------------------------------------
// Constructors / Destructors
// --------------------------------------------------

WindowContext::WindowContext(int width, int height, const char* windowName) {
    initGLFW();
    window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("Could not create a GLFW Window!");
    _ownsGLFW = true;

    useContext();
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Could not initialize GLEW!");
}

WindowContext::WindowContext(WindowContext&& other) : window(other.window), _ownsGLFW(other._ownsGLFW) {
    other.window = NULL;
    other._ownsGLFW = false;
}

WindowContext::~WindowContext() {
    if (window != NULL)
        glfwDestroyWindow(window);
    if (_ownsGLFW)
        terminateGLFW();
}

// --------------------------------------------------
// operator overloads
// --------------------------------------------------

WindowContext& WindowContext::operator=(WindowContext&& other) {
    if (this != &other) {
        if (window != NULL) {
            glfwDestroyWindow(window);
            if (_ownsGLFW) terminateGLFW();
        }
        window = other.window;
        _ownsGLFW = other._ownsGLFW;
        other.window = NULL;
        other._ownsGLFW = false;
    }
    return *this;
}

};