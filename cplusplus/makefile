CC=g++
INCLUDES=-I$(GLFW3_HOME)/include/
CFLAGS=-std=c++11 
LIBS=-L/home/msmith/local/lib/ -lglfw3 -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lGLEW
cmd_line: main.o tga_reader.o minimum_texture2d.o word_texture.o texturewords.o
	$(CC) main.o tga_reader.o -o tga_term_check
	$(CC) minimum_texture2d.o tga_reader.o $(LIBS) -o min_texture
	$(CC) word_texture.o tga_reader.o texturewords.o $(LIBS) -o word_texture

minimum_texture2d.o: minimum_texture2d.cpp
	$(CC) $(INCLUDES) -c minimum_texture2d.cpp -o minimum_texture2d.o
	
tga_reader.o: tga_reader.cpp
	$(CC) -c tga_reader.cpp -o tga_reader.o

texturewords.o: texturewords.cpp
	$(CC) $(INCLUDES) -c texturewords.cpp -o texturewords.o

word_texture.o: word_texture.cpp
	$(CC) $(INCLUDES) -c word_texture.cpp -o word_texture.o

main.o: main.cpp
	$(CC) -c main.cpp -o main.o
