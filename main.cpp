#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <sys/signal.h>
#include <memory.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <cstdio>
#include <vector>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <algorithm>

const unsigned int g_window_h { 500 };
const unsigned int g_window_w { 500 };

enum PROGRAM_EXIT
{
    PROGRAM_EXIT_SUCCESS = 0,
    PROGRAM_EXIT_FAILED = -1,
    PROGRAM_EXIT_EXCEPTION = -2
};

class Mesh;

class Shader
{
    friend Mesh;

    private:
        unsigned int vertex_shader, fragment_shader, program;
        const char* vertex_context;
        const char* fragment_context;
    public:
        Shader(const char* vertex_file, const char* fragment_file)
        {
            setupContext(vertex_file, &vertex_context);
            setupContext(fragment_file, &fragment_context);

            setupShader();
        };
        ~Shader();
    private:
        void setupContext(const char*, const char**);
        void setupShader();
};

Shader::~Shader()
{
    glDeleteProgram(program);

    delete vertex_context;
    delete fragment_context;
}

void Shader::setupContext(const char* file_name, const char** context)
{
    std::ifstream file(file_name);
    std::stringstream sstr;

    sstr << file.rdbuf();

    *context = sstr.str().c_str();

    file.close();
    sstr.clear();
}

void Shader::setupShader()
{
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_context, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_context, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glLinkProgram(program);
}

class Mesh
{
    public:
        Mesh(std::vector<float>& vertices, std::vector<unsigned int>& indices)
        : vertices(vertices), indices(indices)
        {
            setupMeshes();
        };
        ~Mesh();
    public:
        void Draw(class Shader&);
    private:
        void setupMeshes();
    private:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;
};

Mesh::~Mesh()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::Draw(Shader& shader)
{
    glUseProgram(shader.program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMeshes()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
}

bool g_poly_mode { false };

const float g_vertices[] =
{
    #include "vertices.inc"
};

const unsigned int g_indices[] =
{
    #include "indices.inc"
};

void turn_polygon_mode(int);
void processInput(GLFWwindow*);

int actual_program(int argc, char** argv)
{
    signal(SIGUSR1, turn_polygon_mode);

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(g_window_w, g_window_h, "Window", NULL, NULL);

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    std::vector<float> v;
    std::vector<unsigned int> i;

    std::copy(&g_vertices[0], &g_vertices[sizeof(g_vertices) / sizeof(g_vertices[0])], std::back_inserter(v));
    std::copy(&g_indices[0], &g_indices[sizeof(g_indices) / sizeof(g_indices[0])], std::back_inserter(i));

    Shader shader("vshader.txt", "fshader.txt");
    Mesh m(v, i);

    while(!glfwWindowShouldClose(window))
    {
        if(g_poly_mode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return PROGRAM_EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    try {
        return actual_program(argc, argv);
    } catch(const std::exception& ex) {
        fprintf(stderr, "ERROR: %s\n", ex.what());
        return PROGRAM_EXIT_EXCEPTION;
    }
}

void turn_polygon_mode(int signal)
{
    g_poly_mode = ! g_poly_mode;
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}