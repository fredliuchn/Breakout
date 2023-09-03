#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include "GL/glew.h"
#include "game_object.h"

//���ߴ�С
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
//�����ٶ�
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject 
{
public:
    //��������
    std::string Type;
	//����ʱ��
    float       Duration;	
    bool        Activated;
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture) 
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif