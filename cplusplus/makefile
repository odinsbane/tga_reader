CC=g++
CFLAGS=-std=c++0x 
CFLAGS=-lGLEW 
CFLAGS+=-lglfw3  
CFLAGS+=-lrt  
CFLAGS+=-lXrandr  
CFLAGS+=-lXi  
CFLAGS+=-lGL  
CFLAGS+=-lm  
CFLAGS+=-ldl  
CFLAGS+=-lXrender  
CFLAGS+=-ldrm  
CFLAGS+=-lXdamage  
CFLAGS+=-lX11-xcb  
CFLAGS+=-lxcb-glx  
CFLAGS+=-lXxf86vm  
CFLAGS+=-lXfixes  
CFLAGS+=-lXext  
CFLAGS+=-lX11  
CFLAGS+=-lpthread  
CFLAGS+=-lxcb  
CFLAGS+=-lXau  
CFLAGS+=-lXdmcp 

cmdlinemake: main.o tga_reader.o minimum_texture2d.o
	$(CC) -o cmdtest main.o tga_reader.o
	$(CC) -o minimum_texture tga_reader.o minimum_texture2d.o $(CFLAGS)