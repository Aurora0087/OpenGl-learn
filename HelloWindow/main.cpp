// Include GLEW before GLFW: GLEW supplies the OpenGL function declarations.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace
{
    constexpr int windowWidth = 800;
    constexpr int windowHeight = 600;
    constexpr const char *windowTitle = "my 1st opengl window";

    GLFWwindow *createWindow()
    {
        // Choose X11 before initializing GLFW so GLEW can use GLX.
        // On a Wayland desktop, XWayland supplies X11 support.
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return nullptr;
        }

        // Request OpenGL 3.3 core, without deprecated OpenGL features.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // nullptr arguments mean windowed mode and no shared OpenGL context.
        GLFWwindow *window = glfwCreateWindow(
            windowWidth, windowHeight, windowTitle, nullptr, nullptr);

        if (!window)
        {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
            return nullptr;
        }

        // OpenGL calls on this thread now use this window's context.
        glfwMakeContextCurrent(window);
        return window;
    }

    bool initializeOpenGL()
    {
        // Load OpenGL function addresses only after a context is current.
        glewExperimental = GL_TRUE;
        const GLenum result = glewInit();

        if (result != GLEW_OK)
        {
            // Include the numeric code because some GLEW errors have no message.
            std::cerr << "Failed to initialize GLEW (code " << result << "): "
                      << glewGetErrorString(result) << '\n';
            return false;
        }

        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
        return true;
    }

    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            // Set the close flag so the main loop can finish.
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void renderFrame(GLFWwindow *window)
    {
        // Use framebuffer pixels so the viewport follows resizing and display scaling.
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Set the background color (red, green, blue, alpha), then clear to it.
        glClearColor(0.1f, 0.15f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Present the completed frame by swapping the front and back buffers.
        glfwSwapBuffers(window);
    }
} // namespace: keep these helpers and constants local to this source file.

int main()
{
    GLFWwindow *window = createWindow();
    if (!window)
    {
        return -1;
    }

    if (!initializeOpenGL())
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Keep processing events and drawing until Escape or a close request.
    while (!glfwWindowShouldClose(window))
    {
        // Update keyboard state and handle pending window events.
        glfwPollEvents();
        processInput(window);
        renderFrame(window);
    }

    // Release the window/context before shutting down GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
