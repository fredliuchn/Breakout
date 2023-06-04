#include "SpriteRenderer.h"

#include <iostream>
#include "GL/glew.h"

SpriteRenderer::SpriteRenderer(Shader &shader)
	:m_shader(shader)
{
	initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &m_quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D & texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
	m_shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	
	model = glm::translate(model, glm::vec3(position, 0.0f));

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5*size.y, 0.0f));

	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::translate(model, glm::vec3(-0.5*size.x, -0.5*size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));
	
	m_shader.SetMatrix4("model", model);

	m_shader.SetVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
	unsigned int VBO;
	float vertices[] =
	{
		//pos    //tex
		0.0f,1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,

		0.0f,1.0f,0.0f,1.0f,
		1.0f,1.0f,1.0f,1.0f,
		1.0f,0.0f,1.0f,0.0f
	};

	//创建顶点数组对象
	glGenVertexArrays(1, &m_quadVAO);
	//创建顶点缓冲区对象
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_quadVAO);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	//启用通用顶点数组VAO的 index 索引 对应glVertexAttribPointer的第一个参数
	glEnableVertexAttribArray(0);
	/*
	glVertexAttribPointer第一个参数对应vertexshader里哪个in attribute，是vertexshader中attribute的索引，有如下顶点着色器代码：
	//vertex shader
	layout(location = 0) in vec3 position; //position的location设定为0
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 8, vertexs, GL_STATIC_DRAW);
	*/
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	//解绑VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解绑VAO
	glBindVertexArray(0);
}
