#ifndef MAP_H
#define MAP_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"
#include "SpriteFrame.h"

class SpriteRenderer {
    public:
        SpriteRenderer(Shader& shader, const GLint windowWidth, const GLint windowHeight);
        virtual ~SpriteRenderer();

        void drawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, SpriteFrame frame);
        void drawSprite(Texture& texture, glm::vec3 position, glm::vec2 size, SpriteFrame frame);
    private:
        Shader shader;
        GLuint quadVAO;
        GLint windowWidth, windowHeight;
        GLfloat squareSize;

        void initRenderData();
};

#endif // MAP_H
