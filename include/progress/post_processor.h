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
    //在渲染之前准备framebuffer
    void BeginRender();
    //渲染之后，存储渲染数据到一个纹理
    void EndRender();
    //纹理渲染
    void Render(float time);

public:
	//状态
	Shader m_PostProcessingShader;
	Texture2D m_Texture;
	unsigned int m_Width, m_Height;
	//反转场景中的颜色并颠倒x轴和y轴。
	bool Confuse;
	//利用边缘检测卷积核创造有趣的视觉效果，并以圆形旋转动画的形式移动纹理图片，实现“混沌”特效。
	bool Chaos;
	//轻微晃动场景并附加一个微小的模糊效果。
	bool Shake;

private:
	//初始化纹理
	void initRenderData();
private:
    //帧缓冲区
	unsigned int MSFBO; 
	unsigned FBO;
	//渲染缓冲区
    unsigned int RBO;
    unsigned int VAO;
};

#endif