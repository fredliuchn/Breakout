#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include "GL/glew.h"
#include "game_object.h"

//道具大小
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
//道具速度
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject 
{
public:
    //道具类型
    std::string Type;
	//持续时间
    float       Duration;	
    bool        Activated;
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture) 
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif