#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    // joka rivillä x, y -koordinaatit kolmelle eri vertexille
    float positions[6] = {
       -0.2f, -0.2f,
        0.0f,  0.5f,
        0.5f, -0.5f
    };

    unsigned int iBuffer;
    glGenBuffers(1, &iBuffer);  // &iBuffer = generoidun bufferin ID
    glBindBuffer(GL_ARRAY_BUFFER, iBuffer);  // binding = valitaan
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    // indeksi mistä aloitetaan, montako per vertexi, tyyppi, koko, pointteri
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); // VS debug-modessa suhteellinen polku

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // tämä piirtää sen mikä on VIIMEISIMPÄNÄ valittu eli BINDATTU ylempänä
        glDrawArrays(GL_TRIANGLES, 0, 3);  // mitä piirretään, mistä indeksistä aloitetaan, kuinka monta indeksiä

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}