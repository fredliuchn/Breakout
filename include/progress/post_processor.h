#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

class PostProcessor
{
public:
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
public:
    //����Ⱦ֮ǰ׼��framebuffer
    void BeginRender();
    //��Ⱦ֮�󣬴洢��Ⱦ���ݵ�һ������
    void EndRender();
    //������Ⱦ
    void Render(float time);

public:
	//״̬
	Shader m_PostProcessingShader;
	Texture2D m_Texture;
	unsigned int m_Width, m_Height;
	//��ת�����е���ɫ���ߵ�x���y�ᡣ
	bool Confuse;
	//���ñ�Ե������˴�����Ȥ���Ӿ�Ч��������Բ����ת��������ʽ�ƶ�����ͼƬ��ʵ�֡����硱��Ч��
	bool Chaos;
	//��΢�ζ�����������һ��΢С��ģ��Ч����
	bool Shake;

private:
	//��ʼ������
	void initRenderData();
private:
    //֡������
	unsigned int MSFBO; 
	unsigned FBO;
	//��Ⱦ������
    unsigned int RBO;
    unsigned int VAO;
};

#endif