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
#include <map>

//#undef DEBUG
#define DEBUG

bool g_poly_mode { true };

const float g_vertices[] =
{
    #include "vertices.inc"
};

const unsigned int g_indices[] =
{
    #include "indices.inc"
};


float g_y_pos { 0.0f };
float g_z_rot { 0.0f };
float g_camera_dis { 50.0f };

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

    typedef std::map<const char*, glm::mat4> trans_mat;

    private:
        unsigned int vertex_shader, fragment_shader, program;
        #ifdef DEBUG
        const char* vertex_context = "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "uniform mat4 projection;\n"
                                    "uniform mat4 view;\n"
                                    "uniform mat4 model;\n"
                                    "uniform mat4 transformation;\n"
                                    "void main() {\n"
                                    "gl_Position = projection * view * model * transformation * vec4(aPos, 1.0);\n"
                                    "}\n\0";
        const char* fragment_context = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";
        #else
        const char* vertex_context;
        const char* fragment_context;
        #endif
        std::map<const char*, glm::mat4> transformation_matrices;
    public:
        Shader(const char* vertex_file, const char* fragment_file)
        {
            #ifndef DEBUG
            setupContext(vertex_file, &vertex_context);
            setupContext(fragment_file, &fragment_context);
            #endif

            setupShader();
        };
        ~Shader();
    public:
        void addTransformation(const char*, glm::mat4&);
    private:
        void setupContext(const char*, const char**);
        void setupShader();
};

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::addTransformation(const char* name, glm::mat4& mat)
{
    //if(transformation_matrices.find(name) == transformation_matrices.end())
    //{
        //transformation_matrices.insert(std::make_pair(name, mat));
    //}
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

    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

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
    // how far
    glm::mat4 projection = glm::perspective(glm::radians(g_camera_dis), (float)g_window_w / (float)g_window_h, 0.1f, 100.0f);

    glm::mat4 look = glm::mat4(1.0f);
    // camera view angles, camera pos, camera pos
    look = glm::lookAt(glm::vec3(0.0f, -3.0f, 6.0f), glm::vec3(0.0f, 0.6f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 transl = glm::mat4(1.0f);
    transl = glm::translate(transl, glm::vec3(0.0f, g_y_pos, 1.0f));

    glm::mat4 move = glm::mat4(1.0f);
    move = glm::rotate(move, glm::radians(g_z_rot), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 transformation = transl * move;
    
    glUseProgram(shader.program);

    unsigned int locy = glGetUniformLocation(shader.program, "transformation");
    glUniformMatrix4fv(locy, 1, GL_FALSE, glm::value_ptr(transformation));

    unsigned int loc1 = glGetUniformLocation(shader.program, "projection");
    glUniformMatrix4fv(loc1, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int loc = glGetUniformLocation(shader.program, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(look));

    glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glBindVertexArray(VAO);

    //glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertices) / sizeof(g_vertices[0]));
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

void turn_polygon_mode(int);
void processInput(GLFWwindow*);
void handle_scroll(GLFWwindow*, double, double);

int actual_program(int argc, char** argv)
{
    signal(SIGUSR1, turn_polygon_mode);

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(g_window_w, g_window_h, "Window", NULL, NULL);

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    std::vector<float> v;
    std::vector<unsigned int> i;

    std::copy(&g_vertices[0], &g_vertices[sizeof(g_vertices) / sizeof(g_vertices[0])], std::back_inserter(v));
    std::copy(&g_indices[0], &g_indices[sizeof(g_indices) / sizeof(g_indices[0])], std::back_inserter(i));

    Shader shader("vshader.txt", "fshader.txt");

    Mesh m(v, i);

    glfwSetScrollCallback(window, handle_scroll);

    while(!glfwWindowShouldClose(window))
    {
        if(g_poly_mode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m.Draw(shader);

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            g_y_pos += 0.01f;
        }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            g_y_pos -= 0.01f;
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            g_z_rot += 2.0f;
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            g_z_rot -= 2.0f;
        }

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

void handle_scroll(GLFWwindow* window, double x, double y)
{
    g_camera_dis -= y;
}