#ifndef GAME_H
#define GAME_H

#include "SpriteRenderer.h"
#include "game_level.h"
#include "ball_object.h"

//游戏状态
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};


//表示四个可能的（碰撞）方向
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// 定义一个碰撞状态，是否碰撞，碰撞到物体的什么方向，碰撞到物体上的点位置
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

//游戏的状态与功能
class Game
{
public:
	Game(unsigned int width, unsigned int height);
	~Game();

	//初始化
	void Init();

	//控制输入
	void ProcessInput(float dt);
	
	//更新位置
	void Update(float dt);

	//绘制
	void Render();

	//碰撞检测
	void DoCollisions();

	//碰撞检测
	bool CheckCollision(GameObject &one, GameObject &two);

	Collision CheckCollision(BallObject &one, GameObject &two);

	//计算向量的方向
	Direction VectorDirection(glm::vec2 target);

	//重置关卡
	void ResetLevel();
	
	//重置玩家
	void ResetPlayer();
	std::map<std::string, GameLevel> m_levels;
	std::string m_strlevel;
	SpriteRenderer*         m_SpriteRenderer;
	GameState               m_eState;
	bool                    m_bKeys[1024];
	unsigned int            m_uiWidth, m_uiHeight;
};

#endif