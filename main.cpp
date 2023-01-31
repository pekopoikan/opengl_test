#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <sys/signal.h>
#include <memory.h>

class CTriangleRenderer
{
    public:
        CTriangleRenderer(const float c_vertices[], const char* vertex_shader, const char* fragment_shader)
        :m_v_shader(vertex_shader), m_f_shader(fragment_shader)
        {
            memcpy(m_vertices, c_vertices, 9 * sizeof(float));
            setup_buffers();
            setup_shaders();
        }
    public:
        void setup_buffers()
        {
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(0);
        }
        void setup_shaders()
        {
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &m_v_shader, NULL);
            glCompileShader(vertexShader);

            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &m_f_shader, NULL);
            glCompileShader(fragmentShader);

            shaderProgram = glCreateProgram();

            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);

            glLinkProgram(shaderProgram);
        }
        void render()
        {
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    private:
        unsigned int VBO, VAO;
        unsigned int shaderProgram, vertexShader, fragmentShader;
    private:
        const char* m_v_shader;
        const char* m_f_shader;
        float m_vertices[9] = {};
};

bool g_bDebug { false };

const float vertices[] =
{
    -0.55f,  0.5f, 0.0f,
    -0.7f,  0.0f, 0.0f,
    -0.4f,  0.0f, 0.0f,
    -0.25f, 0.5f, 0.0f,
    -0.1f, 0.0f, 0.0f,
};

unsigned int indices[] =
{
    0, 1, 2,
    2, 3, 4 
};

const float triangle_1_vertices[] = {
    -0.55f,  0.5f, 0.0f,
    -0.7f,  0.0f, 0.0f,
    -0.4f,  0.0f, 0.0f,
};

const float triangle_2_vertices[] = {
    -0.4f,  0.0f, 0.0f,
    -0.25f, 0.5f, 0.0f,
    -0.1f, 0.0f, 0.0f,
};

const char* vertexShaderSource = "#version 330 core\n"
"layout (location) = 0 in vec3 aPos;\n"
"void main() {\n"
"gl_Pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(3.0f, 2.5f, 1.2f, 1.0f);\n"
"}\n\0";

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

int main()
{
    signal(SIGUSR1, turn_on_debugging);

    glfwInit();

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

    #ifdef N
    unsigned int VBO[2], VAO[2];

    glGenBuffers(2, VBO);
    glGenVertexArrays(2, VAO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_1_vertices), triangle_1_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_2_vertices), triangle_2_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);
    #endif

    #ifdef Ex1
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    #endif

    #ifdef N

    unsigned int vertexShader, fragmentShader, fragmentShader2;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);

    unsigned int shaderProgram, shaderProgram2;

    shaderProgram = glCreateProgram();
    shaderProgram2 = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);

    glLinkProgram(shaderProgram2);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(fragmentShader2);
    #endif

    CTriangleRenderer* triangle = new CTriangleRenderer(triangle_1_vertices, vertexShaderSource, fragmentShaderSource);

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

        triangle->render();

        //glUseProgram(shaderProgram);
        //glBindVertexArray(VAO[0]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        //glUseProgram(shaderProgram2); 
        //glBindVertexArray(VAO[1]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    #ifdef N
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgram);
    #endif

    glfwTerminate();

    return 0;
}