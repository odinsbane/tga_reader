#include "tga_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int arg_c, char** args){
    if(arg_c < 2){
	printf("Usage: cmdtest image.tga\n");
	printf("where image.tga is an uncompressed true-color (BGRA - 8 bit per channel) image\n");
	exit(0);
    }
    std::string f = args[1];
    TGA::image* img = TGA::load_texture(f);
    printf("size: %d x %d\n", img->width, img->height);
    int size = img->width*img->height*4;
    printf("first pixel: %d,%d,%d,%d \n",img->texture[0], img->texture[1], img->texture[2], img->texture[3]);
    
    printf("last pixel %d,%d,%d,%d \n",img->texture[size-1-0], img->texture[size-1-1], img->texture[size-1-2], img->texture[size-1-3]);
    
    return 0;
}
