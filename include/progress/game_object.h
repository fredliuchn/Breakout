/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "texture.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, 
		glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
public:

	//物体位置
	glm::vec2   m_Position;

	//物体大小
	glm::vec2 m_Size;

	//物体的运动速度
	glm::vec2 m_Velocity;
    
	//物体颜色
	glm::vec3 m_Color;
    
	//物体旋转角度
	float m_Rotation;

	//物体不可被销毁
    bool m_IsSolid;

	//物体被销毁
    bool m_Destroyed;

	//物体纹理id
    Texture2D m_Sprite; 
public:

    virtual void Draw(SpriteRenderer &renderer);
};

#endif