#include "Mesh3DRenderer.h"

#include <iostream>

Mesh3DRenderer::Mesh3DRenderer(Shader& shader, const char* modelFile, Camera* camera)
    : shader(shader), camera(camera)
{
	this->initRenderDataPhong(modelFile);
}

Mesh3DRenderer::~Mesh3DRenderer() {
	glDeleteVertexArrays(1, &this->quadVAO);
	glDeleteBuffers(2,&VBO_tex);
}

void Mesh3DRenderer::initRenderData(const char* modelFile) {
	// Configure VAO/VBO
	GLuint VBO;
	GLuint EBO;

    std::ifstream infile(modelFile);
    numVertices, numFaces;
    infile >> numVertices >> numFaces;
    GLfloat vertices [numVertices * 6];
    GLint faces [numFaces * 3];
    GLint vcount = 0;

    while(vcount < numVertices){
    	GLfloat vx, vy, vz, _dc;
    	GLint r, g, b;
    	infile >> vx >> vy >> vz >> _dc >> _dc >> _dc >> r >> g >> b;
    	vertices[vcount*6] = vx;
    	vertices[vcount*6+1] = vy;
    	vertices[vcount*6+2] = vz;
    	vertices[vcount*6+3] = (GLfloat) r/255;
    	vertices[vcount*6+4] = (GLfloat) g/255;
    	vertices[vcount*6+5] = (GLfloat) b/255;
    	vcount++;
    }
    GLint fcount = 0;
    while(fcount < numFaces){
    	GLint v1, v2, v3, _dc;
    	infile >> _dc >> v1 >> v2 >> v3;
    	faces[fcount*3] = v1;
    	faces[fcount*3+1] = v2;
    	faces[fcount*3+2] = v3;
    	fcount++;
    }
    infile.close();

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void Mesh3DRenderer::initRenderDataPhong(const char* modelFile) {
	// Configure VAO/VBO
	GLuint VBO;
	GLuint EBO;

    std::ifstream infile(modelFile);
    numVertices, numFaces;
    infile >> numVertices >> numFaces;
    GLfloat vertices [numVertices * 9];// Position (3) + normal(3) + color(3)
    GLint faces [numFaces * 3];
    GLint vcount = 0;

    while(vcount < numVertices){
    	GLfloat vx, vy, vz, nx, ny, nz;
    	GLint r, g, b;
    	infile >> vx >> vy >> vz >> nx >> ny >> nz >> r >> g >> b;
    	vertices[vcount*9] = vx;
    	vertices[vcount*9+1] = vy;
    	vertices[vcount*9+2] = vz;
    	vertices[vcount*9+3] = nx;
    	vertices[vcount*9+4] = ny;
    	vertices[vcount*9+5] = nz;
    	vertices[vcount*9+6] = (GLfloat) r/255;
    	vertices[vcount*9+7] = (GLfloat) g/255;
    	vertices[vcount*9+8] = (GLfloat) b/255;
    	vcount++;
    }
    GLint fcount = 0;
    while(fcount < numFaces){
    	GLint v1, v2, v3, _dc;
    	infile >> _dc >> v1 >> v2 >> v3;
    	faces[fcount*3] = v1;
    	faces[fcount*3+1] = v2;
    	faces[fcount*3+2] = v3;
    	fcount++;
    }
    infile.close();

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void Mesh3DRenderer::setShader(Shader& shader) {
    this->shader = shader;
}

void Mesh3DRenderer::draw(glm::mat4 model, glm::vec3 objPos) {// glm::vec2 img_size

	this->shader.use();
    this->shader.setVector3f("objPos", objPos);
	this->shader.setMatrix4("view", glm::mat4(1.0));
		

	if (camera->isEnabled()){
		// View Matrix
		this->shader.setMatrix4("view", this->camera->getViewMatrix());

		// Projection Matrix
		glm::mat4 projection = this->camera->getPerspective();
		this->shader.setMatrix4("projection", projection);

	} else {
		glm::mat4 projection = this->camera->getOrthogonal();
		this->shader.setMatrix4("projection", projection);
	}


	this->shader.setMatrix4("model", model);

	glBindVertexArray(this->quadVAO);
	glDrawElements(GL_TRIANGLES, numFaces*3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
