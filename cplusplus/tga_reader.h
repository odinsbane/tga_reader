#ifndef TGA_READER
#define TGA_READER
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <GL/gl.h>
namespace TGA{
    struct image{
        int height;
        int width;
        GLubyte* texture;
    };

    image* load_texture(std::string &filename);
}







#endif
