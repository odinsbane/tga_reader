#ifndef TGA_READER
#define TGA_READER
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#ifdef __APPLE__
  #include <OpenGL/gltypes.h>
  #else
  #include <GL/gl.h>
#endif
namespace TGA{
    struct image{
        int height;
        int width;
        int pixel_depth;
        GLubyte* texture;
    };

    image* load_texture(std::string &filename);
}







#endif
