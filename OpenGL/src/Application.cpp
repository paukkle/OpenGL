#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();  // palauttaa pointterin sourcen alkuun. sourcen on oltava olemassa!

    // shader,  montakoo sorsaa, sorsan pointterin muistiosoite, pituus (jos null, niin oletetaan ett‰ on null-terminated)
    glShaderSource(id, 1, &src, nullptr); // m‰‰rittelee shaderin l‰hteen

    glCompileShader(id);

    int result;

    // i = id, v=vektori
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);  // palauttaa shaderin statuksen resulttiin
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
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

    // Voidaan tuhota, koska n‰m‰ on jo linkitetty ohjelmaan
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

    // joka rivill‰ x, y -koordinaatit kolmelle eri vertexille
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
    // indeksi mist‰ aloitetaan, montako per vertexi, tyyppi, koko, pointteri
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


    std::string vertexShader =
        "#version 410 core\n"
        "\n"
        "layout(location = 0) in vec4 position;"  // location, glVertexAttribPointer eka argumentti
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";


    std::string fragmentShader =
        "#version 410 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"  // location, glVertexAttribPointer eka argumentti
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // t‰m‰ piirt‰‰ sen mik‰ on VIIMEISIMPƒNƒ valittu eli BINDATTU ylemp‰n‰
        glDrawArrays(GL_TRIANGLES, 0, 3);  // mit‰ piirret‰‰n, mist‰ indeksist‰ aloitetaan, kuinka monta indeksi‰

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glDeleteShader(shader);

    glfwTerminate();
    return 0;
}