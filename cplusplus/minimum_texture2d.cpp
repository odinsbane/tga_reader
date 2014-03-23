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

//two triangles put together to make a square
const float positions[] = {
    -1, -1, 0,
    -1, 1, 0,
    1, -1, 0,
    1, -1, 0,
    -1, 1, 0,
    1, 1, 0
};

const int indices[] = {
    1,2,3,4,5,6
};


//simple shader output = input. texture coordinates are the xy, should make 4 copies of texture.
const std::string vertexStr(
#ifdef __APPLE__
        "#version 150\n"
#else
		"#version 130\n"
#endif
        "in vec3 pos;\n"
        "out vec2 texCoords;\n"
        "void main(){\n"
        "   texCoords=vec2(pos.x, pos.y);\n"
        "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
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
int main(int arg_co, char** args){
        if(arg_co < 2){
                printf("Usage: cmdtest image.tga\n");
                printf("where image.tga is an uncompressed true-color (BGRA - 8 bit per channel) image\n");
                exit(0);
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

        printf("GLSL version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("GL Version: %s\n", glGetString(GL_VERSION));

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
        /*
         * Set up the buffers.
        */
        glUseProgram(program);
	
	
        GLuint positionBufferObject;
        glGenBuffers(1, &positionBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*3,positions, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLuint indexBufferObject;
        glGenBuffers(1, &indexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*6, indices, GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        GLuint positionAttribute = glGetAttribLocation(program, "pos");


	

        GLuint vao;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

        glBindVertexArray(0);
        glUseProgram(0);

        std::string input = args[1];
        TGA::image* img = TGA::load_texture(input);
        printf("size: ( %d x %d ) bit per pixel: %d\n", img->width, img->height, img->pixel_depth);
        
        GLuint pixel_type = img->pixel_depth==32?GL_BGRA:GL_BGR;
        /*
         * Set up texture buffer
         **/
        glUseProgram(program);
        GLuint texBufferdObject;
        glGenTextures(1, &texBufferdObject);
        glBindTexture(GL_TEXTURE_2D, texBufferdObject);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, pixel_type, GL_UNSIGNED_BYTE, img->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        const int texUnit=0;
        GLuint samplerUniform = glGetUniformLocation(program, "texSampler");
        glUniform1i(samplerUniform, texUnit);
        glUseProgram(0);

        /* Loop until the user closes the window */
        printf("to here\n");
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);
            glBindVertexArray(vao);

            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, texBufferdObject);

            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }


        glfwTerminate();
        return 0;

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

