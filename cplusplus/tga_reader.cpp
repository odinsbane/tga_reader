#include "tga_reader.h"

namespace TGA{
    /**
     * This is intended to load TGA files. This only supports non-compressed true color
     * tga files. The resulting texture will have BGRA ordered pixels.
     * */
    image* load_texture(std::string &file_name){
	std::ifstream infile( file_name.c_str(), std::ifstream::binary );
        
        char* header = new char[18];
        //infile.seekg(0, infile.beg);
        
        if(infile){
            infile.read(header, 18);
        }
        
        
        image* img = new image;
        img->width = ((unsigned char)header[13]<<8) + (unsigned char)header[12];
        img->height = ((unsigned char)header[15]<<8) + (unsigned char)header[14];
        img->pixel_depth = (unsigned char)header[16];
        int size = img->pixel_depth/8*img->width*img->height;
        
        char* back = new char[size];
        
        infile.read(back,size);
        
        img->texture = (GLubyte*)back;
        
        
        return img;
    }
    
}
