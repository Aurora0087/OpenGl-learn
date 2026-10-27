// Include GLEW before GLFW: GLEW supplies the OpenGL function declarations.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace
{
    constexpr int windowWidth = 800;
    constexpr int windowHeight = 600;
    constexpr const char *windowTitle = "my 1st opengl window";

    constexpr GLfloat verts[] = {
        +0.0f,+1.0f,
        -1.0f,-1.0f,
        +1.0f,-1.0f,
    };

    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    GLuint shaderProgram = 0;

    GLuint compileShader(GLenum type, const char *source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[1024];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            std::cerr << "Shader compilation failed: " << log << '\n';
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    bool initializeTriangle()
    {
        const char *vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec2 position;
            void main()
            {
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )";
        const char *fragmentSource = R"(
            #version 330 core
            out vec4 color;
            void main()
            {
                color = vec4(1.0, 0.5, 0.5, 1.0);
            }
        )";

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (!vertexShader || !fragmentShader)
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint success = GL_FALSE;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[1024];
            glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
            std::cerr << "Shader linking failed: " << log << '\n';
            glDeleteProgram(shaderProgram);
            return false;
        }

        // The VAO remembers how attribute 0 reads positions from the buffer.
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        return true;
    }

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

        // Request OpenGL 3.3 core
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

        glUseProgram(shaderProgram);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

    if (!initializeTriangle())
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

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteProgram(shaderProgram);

    // Release the window/context before shutting down GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
