g++ -framework opengl main.cpp tga_reader.cpp -o cmd_line_check
g++ -I"$GLFW3_HOME/include" -L"$GLFW3_HOME/lib" -framework iokit -framework cocoa -framework opengl minimum_texture2d.cpp tga_reader.cpp -lglfw3 -o minimum_texture