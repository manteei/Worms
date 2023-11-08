#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>

class TextureManager
{
public:
    GLuint LoadTexture(sf::String name);
    void createBox(GLuint skybox[], float size);
};