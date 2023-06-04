#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "progress/game_object.h"
#include "SpriteRenderer.h"
#include "resource_manager.h"


class GameLevel
{
public:
    
    GameLevel() { }

    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);

    void Draw(SpriteRenderer &renderer);

    bool IsCompleted();

public:
	std::vector<GameObject> m_Bricks;
private:

    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif