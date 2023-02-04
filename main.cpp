#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <sys/signal.h>
#include <memory.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

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
    -0.2f, 0.5f, 0.0f,
    -0.2f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.0f,
    0.2f, 0.5f, 0.0f,
};

const unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

const float texCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
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

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    //v = glm::scale(v, glm::vec3(2.6f, 1.0f, 1.0f));

    glm::mat4 v = glm::mat4();

    while(!glfwWindowShouldClose(window))
    {
        if(g_bDebug) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time = glfwGetTime();
        float green = (sin(time) / 2.0f) + 0.5f;

        glUseProgram(program);

        v = glm::rotate(v, 0.08f * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //v = glm::translate(v, glm::vec3(0.001f, 0.0f, 0.0f));
        //v = glm::scale(v, glm::vec3(1.0f, 1.0f, 1.0f));

        double ac = glm::cos(time * glm::radians(20.0f));
        double as = glm::sin(time * glm::radians(20.0f));

        v[0][0] = ac;
        v[0][2] = as;
        v[1][1] = 1;
        v[2][0] = -1 * as;
        v[2][2] = ac;
        v[3][3] = 1;

        unsigned int locamat = glGetUniformLocation(program, "transform");
        glUniformMatrix4fv(locamat, 1, GL_FALSE, glm::value_ptr(v));

        unsigned int location = glGetUniformLocation(program, "gColor");
        glUniform4f(location, 0.0, green, 0.0, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}