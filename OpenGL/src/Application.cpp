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
#include "Shader.h"

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
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        IndexBuffer ib(indices, 6);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();
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
            shader.Bind();
            shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);

            // 2. bind vertex array
            //GLCall(glBindVertexArray(vao));
        
            // bind vertex array
            va.Bind();

            // 3. bind index buffer
            ib.Bind();

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
    }


    glfwTerminate();
    return 0;
}