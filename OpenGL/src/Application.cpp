#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


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
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];  // vertexille ja fragmentille omat streamit
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)  // npos = invalid string position
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();  // palauttaa pointterin sourcen alkuun. sourcen on oltava olemassa!

    // shader,  montakoo sorsaa, sorsan pointterin muistiosoite, pituus (jos null, niin oletetaan että on null-terminated)
    glShaderSource(id, 1, &src, nullptr); // määrittelee shaderin lähteen

    glCompileShader(id);

    int result;

    // i = id, v=vektori
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);  // palauttaa shaderin statuksen resulttiin
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }
    return id;
}


// parametrit sorsakoodia
// OpenGL lukee shaderit, linkkaa ne yhteen ja palauttaa identifierin bindausta varten
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Voidaan tuhota, koska nämä on jo linkitetty ohjelmaan
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(2);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    // blokin ansiosta ohjelma lopetetaan kun piirtämisikkuna on suljetaan
    // muuten tulee gl Error koska ei ole kontekstia
    {
        // joka rivillä x, y -koordinaatit kolmelle eri vertexille
        float positions[] = {
           -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
           -0.5f,  0.5f,
        };

        unsigned int indices[] = {
            0, 1, 2,   // kolmion kulmat positions-rivit
            2, 3, 0
        };

        // vertex array object id
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));  // luodaan yksi array ja tallennetaan sen id vao:on
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vebu(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vebu, layout);


        IndexBuffer ibu(indices, 6);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); // VS debug-modessa suhteellinen polku

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));  // bindataan shader

        GLCall(int location = glGetUniformLocation(shader, "u_Color"));  // haetaan u_Color-muuttujan sijainti
        ASSERT(location != -1);
        // sijainti, värikoodi
        GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));   // asetetaan data u_Coloriin

        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float red = 0.0f;
        float red_increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            // 1. bind shader
            GLCall(glUseProgram(shader));   
            GLCall(glUniform4f(location, red, 0.3f, 0.8f, 1.0f));   // asetetaan data u_Coloriin

            // 2. bind vertex array
            //GLCall(glBindVertexArray(vao));
        
            // bind vertex array
            va.Bind();

            // 3. bind index buffer
            ibu.Bind();

            // tämä piirtää sen mikä on VIIMEISIMPÄNÄ valittu eli BINDATTU ylempänä
            //glDrawArrays(GL_TRIANGLES, 0, 6);  // mitä piirretään, mistä indeksistä aloitetaan, kuinka monta indeksiä
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));  // ei tarvitse laittaa pointteria iboon koska on bindattu
            /* Swap front and back buffers */

            if (red > 1.0f)
                red_increment = -0.05f;
            else if (red < 0.0f)
                red_increment = 0.05f;

            red += red_increment;

            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        GLCall(glDeleteProgram(shader));
    }


    glfwTerminate();
    return 0;
}