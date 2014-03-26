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
#include <math.h>
#ifndef TEXTUREWORDS
#define TEXTUREWORDS

class Word{
    float* positions;
    float* textureCoordinates;
    GLuint positionBufferObject;
    GLuint texCoordinatesBO;
    GLuint indexBufferObject;
    int* indices;
    float letter_width = 19.25/500.5;
    float letter_height = 40.0/80.0;
    const int texUnit=0;
    static TGA::image* img;
    static std::string alphabet_filename;
    GLuint vao, texBufferedObject;
    int character_count;
    std::string text;
    float origin_x, origin_y;
    GLuint programRef;
    void setPositions();
    void refreshBuffer();
    void shutdown();
public:
    Word(std::string text);
    void prepareBuffers(GLuint &program);
    void draw(GLuint &program);
    void getLetter(char c, float* offsetx, float* offsety);
    void setOrigin(double x, double y);
    ~Word(){
        shutdown();
    };
    
};

#endif