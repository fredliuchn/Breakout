#include <iostream>

#include "GL/glew.h"

#include "texture.h"

Texture2D::Texture2D()
	:m_Width(0), m_Height(0), m_InternalFormat(GL_RGB), m_ImageFormat(GL_RGB)
	, m_WrapS(GL_REPEAT), m_WrapT(GL_REPEAT), m_FilterMin(GL_LINEAR), m_FilterMax(GL_LINEAR)
{
	glGenTextures(1, &this->m_ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
	this->m_Width = width;
	this->m_Height = height;

	glBindTexture(GL_TEXTURE_2D, m_ID);

	// ����Ĳ�� ����������OpenGL������α�ʾ ����Ŀ�� ����Ŀ�ȸ߶� ����ı߽� ��������ĸ�ʽ һ��������ÿ��������ռ�õĿռ��С �ⲿ�������ݵ���ʼ��ַ
	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_ImageFormat, GL_UNSIGNED_BYTE, data);

	//X����ѭ��ʹ������ 
	//GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, GL_MIRROR_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);

	//Y����ѭ��ʹ������
	//GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, GL_MIRROR_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);

	//������˷�ʽ  ѹ����ʽ
	//GL_NEAREST GL_LINEAR GL_NEAREST_MIPMAP_NEAREST GL_LINEAR_MIPMAP_NEAREST GL_NEAREST_MIPMAP_LINEAR GL_LINEAR_MIPMAP_LINEAR	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMin);
	
	//������˷�ʽ  ���췽ʽ
	//GL_NEAREST GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMax);

	//����֮ǰ������󶨹�ϵ��ʹOpenGL�������״̬�ָ���Ĭ��״̬
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}
