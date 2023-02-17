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

glm::vec3 g_scale(0.005f, 0.005f, 0.005f);

glm::vec3 g_view(0.0f, 0.0f, -3.0f);

glm::vec3 g_pos(0.0f, 0.0f, 0.0f);

glm::vec3 g_rot(0.0f, 0.0f, 1.0f);

glm::vec3 g_model(0.0f, 1.0f, 0.0f);
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

class Shader
{
    typedef std::map<const char*, glm::mat4> trans_mat;
    public:
        unsigned int program;
    private:
        unsigned int vertex_shader, fragment_shader;
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
    private:
        void setupContext(const char*, std::string&);
        void setupShader();
};

Shader::~Shader()
{
    glDeleteProgram(program);
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

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normals;
    glm::vec2 Coordinates;
};

struct Texture
{
    unsigned int id;
};

class Mesh
{
    public:
        std::vector<Vertex> vertices;
        std::vector<Texture> textures;
        std::vector<unsigned int> indices;
    public:
        Mesh()
        {
            fprintf(stdout, "Mesh constructor called\n");
        };
    public:
        void Draw(Shader&);
        void setup();
    private:
        unsigned int VAO, VBO, EBO;
};

void Mesh::setup()
{
    fprintf(stdout, "vertices size %i\n", vertices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normals));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Coordinates));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(0);
}

class Model
{
    private:
        std::vector<Mesh> meshes;
    public:
        void loadModel(std::string&);
        void Draw(Shader&);
    private:
        Mesh processMesh(aiMesh*, const aiScene*);
};

void Model::loadModel(std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if(!scene) {
        fprintf(stdout, "Couldnt import model file\n");
    } else {
        for(unsigned i = 0; i < scene->mNumMeshes; i++)
        {
            meshes.push_back(processMesh(scene->mMeshes[i], scene));
        }
    }

    fprintf(stdout, "Meshes size %i\n", meshes.size());
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    Mesh m;

    // vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        if(mesh->HasNormals())
        {
            vertex.Normals.x = mesh->mNormals[i].x;
            vertex.Normals.y = mesh->mNormals[i].y;
            vertex.Normals.z = mesh->mNormals[i].z;
        }

        if(mesh->mTextureCoords[0]) {
            vertex.Coordinates.x = mesh->mTextureCoords[0][i].x;
            vertex.Coordinates.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.Coordinates = glm::vec2(0.0f, 0.0f);
        }

        m.vertices.push_back(vertex);
    }

    //loading textures
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::map<aiTextureType, const char*> tex_types = {
        {aiTextureType_DIFFUSE, "texture_diffuse"},
        {aiTextureType_SPECULAR, "texture_specular"},
        {aiTextureType_HEIGHT, "texture_height"},
        {aiTextureType_AMBIENT, "texture_normal"}};

    for(std::pair<aiTextureType, const char*> p : tex_types)
    {
        for(unsigned int i = 0; i < material->GetTextureCount(p.first); i++)
        {
            aiString str;
            material->GetTexture(p.first, i, &str);

            Texture tex;

            unsigned int texID;
            glGenTextures(1, &texID); //

            int w, h, nr;

            unsigned char* data = stbi_load(str.C_Str(), &w, &h, &nr, 0);

            if(data) {
                glBindTexture(GL_TEXTURE_2D, texID);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            } else {
                fprintf(stdout, "Failed to load texture\n");
            }

            stbi_image_free(data);

            tex.id = texID;

            m.textures.push_back(tex);
        }
    }

    // indices
    for(unsigned int f = 0; f < mesh->mNumFaces; f++)
    {
        aiFace face = mesh->mFaces[f];
        for(unsigned int _f = 0; _f < face.mNumIndices; _f++)
        {
            m.indices.push_back(face.mIndices[_f]);
        }
    }

    m.setup();

    return m;
}

void Model::Draw(Shader& shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
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
    std::vector<Texture> t;

    //std::copy(&g_vertices[0], &g_vertices[sizeof(g_vertices) / sizeof(g_vertices[0])], std::back_inserter(v));
    //std::copy(&g_indices[0], &g_indices[sizeof(g_indices) / sizeof(g_indices[0])], std::back_inserter(i));

    std::string mdl("Darunia.obj");

    Shader shader("vshader.vs", "fshader.fs");

    Model m;
    m.loadModel(mdl);

    glfwSetScrollCallback(window, handle_scroll);

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
            ImGui::SliderFloat3("Scale", glm::value_ptr(g_scale), -100.0f, 100.0f);
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

        glUseProgram(shader.program);

        glm::mat4 projection = glm::perspective(glm::radians(g_camera_dis), (float) g_window_w / (float) g_window_h, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, g_view);
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, g_pos);
        glm::mat4 scale = glm::mat4(1.0f);
        scale = glm::scale(scale, g_scale);
        translate *= scale;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(g_model_angle), g_model);

        unsigned int l_transformation = glGetUniformLocation(shader.program, "transformation");
        glUniformMatrix4fv(l_transformation, 1, GL_FALSE, glm::value_ptr(translate));
        unsigned int l_projection = glGetUniformLocation(shader.program, "projection");
        glUniformMatrix4fv(l_projection, 1, GL_FALSE, glm::value_ptr(projection));
        unsigned int l_view = glGetUniformLocation(shader.program, "view");
        glUniformMatrix4fv(l_view, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int l_model = glGetUniformLocation(shader.program, "model");
        glUniformMatrix4fv(l_model, 1, GL_FALSE, glm::value_ptr(model));

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