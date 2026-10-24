// GLEW declares OpenGL functions and loads their addresses when glewInit() is called.
// Include it before GLFW to avoid conflicting OpenGL headers.
#include <GL/glew.h>
// GLFW provides windows, OpenGL contexts, and keyboard/mouse input.
#include <GLFW/glfw3.h>
// Provides std::cout for normal output and std::cerr for errors.
#include <iostream>

// Program entry point; argc and argv contain command-line arguments (unused here).
int main(int argc, char const *argv[])
{
    // Initialize GLFW; ! means enter this block if initialization fails.
    if (!glfwInit())
    {
        std::cerr << "Failed to init  GLFW\n";
        // Exit the program with a nonzero value to indicate failure.
        return -1;
    }

    // Request OpenGL version 3.3 for the next window we create.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use the core profile, which excludes old, deprecated OpenGL features.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create an 800 x 600 window with this title and an OpenGL context.
    // First nullptr: windowed mode; second nullptr: do not share another context.
    // A tiling window manager may resize the window after creation.
    GLFWwindow *window = glfwCreateWindow(
        800, 600, "my 1st opengl window", nullptr, nullptr);

    // A null pointer means window or context creation failed.
    if (!window)
    {
        std::cerr << "Fail to create window\n";
        // Release GLFW resources before exiting after a failure.
        glfwTerminate();
        // Exit the program with a nonzero value to indicate failure.
        return -1;
    }

    // Make this window's OpenGL context active on this thread before OpenGL calls.
    glfwMakeContextCurrent(window);

    // Ask GLEW to expose functions with available entry points, including ones
    // whose extensions are not advertised. This does not initialize GLEW itself.
    // NOTE: This file currently has no glewInit() call.
    glewExperimental = GL_TRUE;
    
    // Request X11 (through XWayland on a Wayland desktop) for GLFW initialization.
    // NOTE: This hint must be set BEFORE the first glfwInit() to select X11.
    // Here GLFW is already initialized, so this does not switch the current window.
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    // GLFW is already initialized: this second call succeeds without reinitializing it.
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        // Exit the program with a nonzero value to indicate failure.
        return -1;
    }

    // Query the active context's OpenGL version and print it.
    std::cout << "OpenGl version:" << glGetString(GL_VERSION) << "\n";

    // Render frames until the window's close flag is set.
    while (!glfwWindowShouldClose(window))
    {
        // Check whether Escape is currently held down.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            // Request that the render loop ends on its next condition check.
            glfwSetWindowShouldClose(window, true);
        }

        // Map OpenGL drawing to this pixel rectangle in the framebuffer.
        // These fixed dimensions do not automatically follow window resizing.
        glViewport(0, 0, 800, 600);
        // Set the clear color: red, green, blue, alpha (values from 0 to 1).
        glClearColor(0.1f, 0.15f, 0.25f, 1.0f);
        // Fill the color buffer with the chosen background color.
        glClear(GL_COLOR_BUFFER_BIT);

        // Present the completed back buffer by swapping it with the front buffer.
        glfwSwapBuffers(window);
        // Process pending input and window events, updating GLFW's input state.
        glfwPollEvents();
    }

    // Destroy the window and its OpenGL context after the loop finishes.
    glfwDestroyWindow(window);
    // Release GLFW's remaining resources.
    glfwTerminate();

    // Report successful completion to the operating system.
    return 0;
}
