#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

struct Character {
    unsigned int TextureID;
	//字形的大小
    glm::ivec2   Size;
	//字形左侧/顶部的偏移量
    glm::ivec2   Bearing;
	//水平偏移量
    unsigned int Advance;
};

class TextRenderer
{
public:
    std::map<char, Character> Characters; 

    Shader TextShader;

    TextRenderer(unsigned int width, unsigned int height);

    void Load(std::string font, unsigned int fontSize);

    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
    unsigned int VAO, VBO;
};

#endif 