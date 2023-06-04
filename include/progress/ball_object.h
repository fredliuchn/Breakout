/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

class BallObject : public GameObject
{
public:
	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
public:
    //球半径
    float m_fRadius;

	//球在挡板上
    bool m_bStuck;

	bool m_bSticky;

	bool m_bPassThrough;

    //移动球并保证球在窗口范围内
    glm::vec2 Move(float dt, unsigned int window_width);

    // resets the ball to original state with given position and velocity
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif