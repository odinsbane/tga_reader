#include "texturewords.h"

std::string Word::alphabet_filename ="alphabet.tga";
TGA::image* Word::img = TGA::load_texture(alphabet_filename);

/*

 A short class for displaying strings in OpenGL + GLFW3. An alphabet of mono space font is stored in a .tga file.
 The alphabet is loaded when the program is ran and each character is displayed by using 2 triangles with
 
 @todo: map displayable ascii characters. Currently only does a-z, A-Z.
 @todo: fix scaling. If the window is not square the letters are stretched.
 @todo: add color.
 @todo: be able to get values, eg width, height.
 
 */
Word::Word(std::string t){
    text = t;
    int count = (int)text.length();
    character_count = count;
    
    positions = new float[4*count*2];
    indices = new int[6*count];
    textureCoordinates = new float[4*count*2];
    
    origin_x = 0;
    origin_y = 0;
    float ox, oy;
    for(int i = 0; i<count; i++){
        float originx = i*letter_width;
        float* pos = &positions[8*i];
        int* dice = &indices[6*i];
        float* tex = &textureCoordinates[8*i];
        
        
        //-1,-1
        pos[0] = originx;
        pos[1] = 0;
        
        //-1, 1
        pos[2] = originx;
        pos[3] = letter_height;
        
        //1, 1
        pos[4] = originx + letter_width;
        pos[5] = letter_height;
        
        //1, -1
        pos[6] = originx + letter_width;
        pos[7] = 0;
        
        int offset = i*4;
        dice[0] = 0+offset;
        dice[1] = 1+offset;
        dice[2] = 3+offset;
        dice[3] = 3+offset;
        dice[4] = 1+offset;
        dice[5] = 2+offset;
        
        getLetter(text[i], &ox, &oy);
        printf("letter: %c at %f, %f\n", text[i], ox, oy);
        tex[0] = ox;
        tex[1] = oy;
        
        tex[2] = ox;
        tex[3] = oy + letter_height;
        
        tex[4] = ox + letter_width;
        tex[5] = oy + letter_height;
        
        tex[6] = ox + letter_width;
        tex[7] = oy;
       
    }
    programRef=0;
    
}

/*
 Called whenever the position array is changed to rebuffer the positions. returns immediately
 if the buffers have not been prepared yet.
 */
void Word::refreshBuffer(){
    if(programRef==0) return;
    
    glUseProgram(programRef);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*8*character_count, positions);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

/*
   Loads the texture, positions and indices into the gl context.
 */
void Word::prepareBuffers(GLuint &program){
    programRef = program;
    
    glUseProgram(program);
	
	
    
    
    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*character_count*2,positions, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &texCoordinatesBO);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordinatesBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*character_count*2,textureCoordinates, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*6*character_count, indices, GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
    GLuint positionAttribute = glGetAttribLocation(program, "pos");
    GLuint texCoordintesAttribute = glGetAttribLocation(program, "tc");
    
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, texCoordinatesBO);
    glEnableVertexAttribArray(texCoordintesAttribute);
    glVertexAttribPointer(texCoordintesAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    
    glBindVertexArray(0);
    glUseProgram(0);
    printf("%d,%d,%d,%d", img->texture[0], img->texture[1], img->texture[2], img->texture[3]);
    GLuint pixel_type = img->pixel_depth==32?GL_BGRA:GL_BGR;
    printf("pixel_depth %d\n",img->pixel_depth);
    /*
     * Set up texture buffer
     **/
    
    glUseProgram(program);
    glGenTextures(1, &texBufferedObject);
    glBindTexture(GL_TEXTURE_2D, texBufferedObject);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, pixel_type, GL_UNSIGNED_BYTE, img->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    GLuint samplerUniform = glGetUniformLocation(program, "texSampler");
    glUniform1i(samplerUniform, texUnit);
    glUseProgram(0);

}

/*
   Draws the word using the provided program.
*/
void Word::draw(GLuint &program){
    glUseProgram(program);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texBufferedObject);
    
    glDrawElements(GL_TRIANGLES, character_count*6, GL_UNSIGNED_INT, 0);
    
    //glDrawArrays(GL_TRIANGLES, 0, character_count*6);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

/*
 Finds the offset of the texture for the current letter.
 */
void Word::getLetter(char c, float* offsetx, float* offsety){
    printf("%d ", c);
    if((c>=65) & (c<91)){
        *offsetx = (c - 65)*letter_width;
        *offsety = letter_height;
    } else if((c>=97) & (c<123)){
        *offsetx = (c - 97)*letter_width;
        *offsety = 0;
    }
    
}

/*
 Clean up. Deletes backing arrays and asks gl to delete bufferes.
 */
void Word::shutdown(){
    
    glDeleteBuffers(1, &positionBufferObject);
    glDeleteBuffers(1, &texCoordinatesBO);
    glDeleteBuffers(1, &indexBufferObject);
    delete[] positions;
    delete[] indices;
    delete[] textureCoordinates;
    
}

/*
    Sets the bottom left position of the string.
 */
void Word::setOrigin(double x, double y){
    origin_x = x;
    origin_y = y;
    for(int i = 0; i<character_count; i++){
        float originx = i*letter_width;
        float* pos = &positions[8*i];
        
        
        //-1,-1
        pos[0] = origin_x + originx;
        pos[1] = origin_y + 0;
        
        //-1, 1
        pos[2] = origin_x + originx;
        pos[3] = origin_y + letter_height;
        
        //1, 1
        pos[4] = origin_x + originx + letter_width;
        pos[5] = origin_y + letter_height;
        
        //1, -1
        pos[6] = origin_x + originx + letter_width;
        pos[7] = origin_y + 0;
        
        
    }
    
    refreshBuffer();
    
    
}