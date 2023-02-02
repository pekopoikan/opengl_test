#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <sys/signal.h>
#include <memory.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

bool g_bDebug {false};

void set_view_port(GLFWwindow* window, int z, int v)
{
    glViewport(0, 0, z, v);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void turn_on_debugging(int sig)
{
    g_bDebug = ! g_bDebug;
}

const char* vertexShader =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;"
"void main() {\n"
"gl_Position = transform * vec4(aPos, 1.0);\n"
"}\n\0";

const char* fragmentShader =
"#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 gColor;"
"void main() {\n"
"color = gColor;"
"}\n\0";

const float vertices[] = {
    0.0f, 0.5f, 0.0f,
    -0.2f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.0f
};

int main()
{
    signal(SIGUSR1, turn_on_debugging);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(500, 500, "OpenGl Window", NULL, NULL);

    if(window == NULL)
    {
        fprintf(stderr, "Error creating window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, set_view_port);

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    unsigned int vertex_shader, fragment_shader, program;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertexShader, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentShader, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glm::mat4 v = glm::mat4(1.0f);

    while(!glfwWindowShouldClose(window))
    {
        if(g_bDebug) {
            glPolygonMode(GL_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time = glfwGetTime();
        float green = (sin(time) / 2.0f) + 0.5f;

        glUseProgram(program);

        v = glm::rotate(v, 0.03f * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int locamat = glGetUniformLocation(program, "transform");
        glUniformMatrix4fv(locamat, 1, GL_FALSE, glm::value_ptr(v));

        unsigned int location = glGetUniformLocation(program, "gColor");
        glUniform4f(location, 0.0, green, 0.0, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}