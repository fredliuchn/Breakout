#include "progress/ball_object.h"


BallObject::BallObject() 
    : GameObject(), m_fRadius(12.5f), m_bStuck(true), m_bSticky(false), m_bPassThrough(false)  { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), m_fRadius(radius), m_bStuck(true), m_bSticky(false), m_bPassThrough(false) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
	// 如果没有被固定在挡板上
    if (!m_bStuck)
    {
		// 移动球
        m_Position += m_Velocity * dt;
		// 检查是否在窗口边界以外，如果是的话反转速度并恢复到正确的位置
        if (m_Position.x <= 0.0f)
        {
			m_Velocity.x = -m_Velocity.x;
			m_Position.x = 0.0f;
        }
        else if (m_Position.x + m_Size.x >= window_width)
        {
			m_Velocity.x = -m_Velocity.x;
            m_Position.x = window_width - m_Size.x;
        }
        if (m_Position.y <= 0.0f)
        {
			m_Velocity.y = -m_Velocity.y;
            m_Position.y = 0.0f;
        }
    }
    return m_Position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	m_Position = position;
	m_Velocity = velocity;
    m_bStuck = true;
    m_bSticky = false;
    m_bPassThrough = false;

}