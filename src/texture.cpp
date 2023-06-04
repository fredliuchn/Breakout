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

	// 纹理的层次 纹理数据在OpenGL中是如何表示 纹理的宽度 纹理的宽度高度 纹理的边界 载入纹理的格式 一个像素中每个分量所占用的空间大小 外部纹理数据的起始地址
	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_ImageFormat, GL_UNSIGNED_BYTE, data);

	//X方向循环使用纹理 
	//GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, GL_MIRROR_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);

	//Y方向循环使用纹理
	//GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, GL_MIRROR_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);

	//纹理过滤方式  压缩方式
	//GL_NEAREST GL_LINEAR GL_NEAREST_MIPMAP_NEAREST GL_LINEAR_MIPMAP_NEAREST GL_NEAREST_MIPMAP_LINEAR GL_LINEAR_MIPMAP_LINEAR	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMin);
	
	//纹理过滤方式  拉伸方式
	//GL_NEAREST GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMax);

	//打破之前的纹理绑定关系，使OpenGL的纹理绑定状态恢复到默认状态
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}
