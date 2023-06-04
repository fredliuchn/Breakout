#ifndef TEXTURE_H
#define TEXTURE_H

class Texture2D
{
public:
	Texture2D();
	void Generate(unsigned int width, unsigned int height, unsigned char* data);
	void Bind() const;

	unsigned int m_ID;
	unsigned int m_Width, m_Height; 

	unsigned int m_InternalFormat;
	unsigned int m_ImageFormat;

	unsigned int m_WrapS;
	unsigned int m_WrapT;
	unsigned int m_FilterMin;
	unsigned int m_FilterMax;

};

#endif