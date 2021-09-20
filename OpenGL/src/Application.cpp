#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        none = -1, vertex = 0, fragment = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::none;
    while (std::getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos)
            {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::vertex;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::fragment;
            }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(),ss[1].str() };
}

static unsigned int CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(GL_VERTEX_SHADER);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int res;
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);
    
        if (res == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = new char[length];
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "FAILED TO COMPILE SHADER" << std::endl;
            std::cout << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(id);
            delete[] message;
            return 0;
        }


    return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& frangmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(frangmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "OPEN GL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
   
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    std::cout << glGetString(GL_VERSION) << std::endl;
    float positions[6] = 
    { 
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f 
    };
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float),positions,GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);
       
      
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        glFlush();
    }
    //glDeleteProgram(shader);
    glfwTerminate();
    glDeleteProgram(shader);
    return 0;
}