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
#include <string>
#include <signal.h>
#include <algorithm>
#include <map>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

bool g_poly_mode { false };

const float g_vertices[] =
{
    #include "vertices.inc"
};

const unsigned int g_indices[] =
{
    #include "indices.inc"
};

glm::vec3 g_view(0.0f, 0.0f, -3.0f);

glm::vec3 g_pos(0.0f, 0.0f, 0.0f);

glm::vec3 g_rot(0.0f, 0.0f, 1.0f);

glm::vec3 g_model(1.0f, 0.0f, 0.0f);
float g_model_angle {0.0f};

float g_z_rot { 0.0f };
float g_camera_dis { 45.0f };

const unsigned int g_window_h { 500 };
const unsigned int g_window_w { 1000 };

enum PROGRAM_EXIT
{
    PROGRAM_EXIT_SUCCESS = 0,
    PROGRAM_EXIT_FAILED = -1,
    PROGRAM_EXIT_EXCEPTION = -2
};

class ModelLoader
{
    private:
        Assimp::Importer importer;
    public:
        ModelLoader(std::string&);
};

ModelLoader::ModelLoader(std::string& path)
{
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene) {
        fprintf(stdout, "Couldnt import model");
    } else {
        fprintf(stdout, "imported\n");
        fprintf(stdout, "%f\n", scene->mMeshes[0]->mVertices[0].x);
    }
}

class Mesh;

class Shader
{
    friend Mesh;

    typedef std::map<const char*, glm::mat4> trans_mat;

    private:
        unsigned int vertex_shader, fragment_shader, program;
        std::string vertex_context;
        std::string fragment_context;
        std::map<const char*, glm::mat4> transformation_matrices;
    public:
        Shader(const char* vertex_file, const char* fragment_file)
        {
            setupContext(vertex_file, vertex_context);
            setupContext(fragment_file, fragment_context);

            setupShader();
        };
        ~Shader();
    public:
        void addTransformation(const char*, glm::mat4&);
    private:
        void setupContext(const char*, std::string&);
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

void Shader::setupContext(const char* file_name, std::string& context)
{
    std::ifstream file;
    std::stringstream sstr;

    try {
        file.open(file_name, std::ios::out);

        if(!file)
        {
            fprintf(stdout, "error");
            abort();
        }

        sstr << file.rdbuf();

        file.close();

        context = sstr.str();
    } catch (const std::ifstream::failure& ex) {
        fprintf(stdout, "%s", ex.what());
    }
}

void Shader::setupShader()
{
    const char* v_source = vertex_context.c_str();
    const char* f_source = fragment_context.c_str();

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &v_source, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &f_source, NULL);
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
        ~Mesh()
        {
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
            glDeleteTextures(1, &TEX);
        };
    public:
        void Draw(class Shader&);
    private:
        void setupMeshes();
    private:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO, TEX;
        int width, height, nrChannels;
        unsigned char* data;
};

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &TEX);
    glBindTexture(GL_TEXTURE_2D, TEX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(15.0f, 0.0f, 0.0f, 1.0f)));

    data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        fprintf(stdout, "error");
    }
    stbi_image_free(data);
}

void Mesh::Draw(Shader& shader)
{
    glm::mat4 projection = glm::perspective(glm::radians(g_camera_dis), (float)g_window_w / (float)g_window_h, 0.1f, 100.0f);

    glm::mat4 look = glm::mat4(1.0f);
    look = glm::translate(look, g_view);

    glm::mat4 transl = glm::mat4(1.0f);
    transl = glm::translate(transl, g_pos);

    glUseProgram(shader.program);

    unsigned int locy = glGetUniformLocation(shader.program, "transformation");
    glUniformMatrix4fv(locy, 1, GL_FALSE, glm::value_ptr(transl));

    unsigned int loc1 = glGetUniformLocation(shader.program, "projection");
    glUniformMatrix4fv(loc1, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int loc = glGetUniformLocation(shader.program, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(look));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(g_model_angle), g_model);

    glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(GL_TEXTURE_2D, TEX);
    glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
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

    //glEnable(GL_TEXTURE);
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    std::vector<float> v;
    std::vector<unsigned int> i;

    std::copy(&g_vertices[0], &g_vertices[sizeof(g_vertices) / sizeof(g_vertices[0])], std::back_inserter(v));
    std::copy(&g_indices[0], &g_indices[sizeof(g_indices) / sizeof(g_indices[0])], std::back_inserter(i));

    Shader shader("vshader.vs", "fshader.fs");

    Mesh m(v, i);

    glfwSetScrollCallback(window, handle_scroll);

    std::string mdl("Darunia.obj");
    ModelLoader test(mdl);

    while(!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Debug");
            if(ImGui::Button("Polygon Mode"))
            {
                turn_polygon_mode(0);
            }
            ImGui::SliderFloat("Camera Distance", &g_camera_dis, -100.0f, 100.0f, "%.3f");
            ImGui::SliderFloat3("View", glm::value_ptr(g_view), -100.0f, 100.0f);
            ImGui::SliderFloat3("Pos", glm::value_ptr(g_pos), -100.0f, 100.0f);
            ImGui::SliderFloat3("Model", glm::value_ptr(g_model), -1.0f, 1.0f);
            ImGui::SliderFloat("Model angle", &g_model_angle, -100.0f, 100.0f);
            ImGui::SliderFloat("Rotation angle", &g_z_rot, -100.0f, 100.0f);
            ImGui::SliderFloat3("Rotation", glm::value_ptr(g_rot), 0.0f, 1.0f);
            ImGui::Button("Save Changes");
            ImGui::End();
        }

        if(g_poly_mode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m.Draw(shader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwDestroyWindow(window);
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