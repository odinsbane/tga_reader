#define DEBUG 1
#ifndef __APPLE__
  #include <GL/glew.h>
#else
  #define GLFW_INCLUDE_GLCOREARB
  #define GLFW_NO_GLU
#endif
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "tga_reader.h"
#include "texturewords.h"
#include <math.h>

//simple shader output = input. texture coordinates are the xy, should make 4 copies of texture.
const std::string vertexStr(
#ifdef __APPLE__
        "#version 150\n"
#else
		"#version 130\n"
#endif
        "in vec2 pos;\n"
        "in vec2 tc;"
        "out vec2 texCoords;\n"
        "void main(){\n"
        "   texCoords=tc;\n"
        "   gl_Position = vec4(pos.x, pos.y, 0, 1.0);\n"
        "}\n"
    );

//Gets the values from the texture.
const std::string fragmentStr(
#ifdef __APPLE__
        "#version 150\n"
#else
		"#version 130\n"
#endif
    "out vec4 outputColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D texSampler;\n"
    "void main(){\n"
        "outputColor =  texture(texSampler, texCoords);\n"
    "}\n"
);


bool shaderStatus(GLuint &shader);
bool programStatus(GLuint &program);
GLuint loadProgram();

int main(int arg_co, char** args){
        std::string text;
        if(arg_co < 2){
            text = "t";
        } else{
            text = args[1];
        }
        /*
         *Creating and initlizing a window on mac.
         *
         **/
        
         
        
        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;
	#ifdef __APPLE__
          glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
          glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
          glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
          glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#else
          //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
          //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
          //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
          //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
        
        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        
        #ifndef __APPLE__
          glewInit();
        #endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        printf("GLSL version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("GL Version: %s\n", glGetString(GL_VERSION));

    
        GLuint program = loadProgram();
    
        Word* word =  new Word(text);
        word->setOrigin(-0.5, -0.5);
        word->prepareBuffers(program);
        /* Loop until the user closes the window */
        printf("to here\n");
        float t = 0;
        char dex = 32;
        char input[2];
        input[1] = 0;
        float step = 40*3.14;
        while (!glfwWindowShouldClose(window))
        
        {
            /* Render here */
            glClearColor(0.2f, 0.2f, 0.6f, 0.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            word->draw(program);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
            t++;
            float x = 0.5 * sin(t*0.05);
            float y = 0.5 * cos(t*0.05);
            if(t>step){
                t= 0;
                dex++;
                if(dex==126) dex=32;
                delete word;
                input[0] = dex;
                word = new Word(input);
                word->prepareBuffers(program);
            }
            word->setOrigin(x,y);
        }


        glfwTerminate();
        return 0;

}

GLuint loadProgram(){
    /*
     * Create the program.
     */
    const char* vertexCStr = vertexStr.c_str();
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource(vertexShader, 1, &vertexCStr, NULL);
    glCompileShader( vertexShader );
    if(!shaderStatus(vertexShader)){
        exit(1);
    }
    const char* fragmentCStr = fragmentStr.c_str();
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource(fragmentShader, 1, &fragmentCStr, NULL);
    glCompileShader( fragmentShader );
	
    
    if(!shaderStatus(fragmentShader)){
        exit(1);
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    if(!programStatus(program)){
        exit(1);
    }
    return program;
}
/**
 * @brief shaderStatus
 * For checking if the shader compiled and printing any error messages.
 *
 * @param shader a shader that was compiled or attempted.
 * @return true if the shader didn't fail.
 */
bool shaderStatus(GLuint &shader){
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = "shader";

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
        return false;
    }


    return true;
}

/**
 * @brief programStatus
 *
 * Checks if the program linked ok.
 *
 * @param program
 * @return
 */
bool programStatus(GLuint &program){
    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if(status==GL_FALSE){
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        return false;
    }
    return true;
}

