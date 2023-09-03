#ifndef GAME_H
#define GAME_H

#include "SpriteRenderer.h"
#include "game_level.h"
#include "ball_object.h"
#include "power_up.h"

//��Ϸ״̬
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};


//��ʾ�ĸ����ܵģ���ײ������
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// ����һ����ײ״̬���Ƿ���ײ����ײ�������ʲô������ײ�������ϵĵ�λ��
typedef std::tuple<bool, Direction, glm::vec2> Collision;

//�����İ���
const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);

//��
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

//��Ϸ��״̬�빦��
class Game
{
public:
	Game(unsigned int width, unsigned int height);
	~Game();

	//��ʼ��
	void Init();

	//��������
	void ProcessInput(float dt);
	
	//����λ��
	void Update(float dt);

	//����
	void Render();

	//��ײ���
	void DoCollisions();

	//��ײ���
	bool CheckCollision(GameObject &one, GameObject &two);

	Collision CheckCollision(BallObject &one, GameObject &two);

	//���������ķ���
	Direction VectorDirection(glm::vec2 target);

	//���ùؿ�
	void ResetLevel();
	
	//�������
	void ResetPlayer();

	//����
	void SpawnPowerUps(GameObject &block);
	void UpdatePowerUps(float dt);

	std::map<int, GameLevel> m_levels;
	int m_level;
	std::vector<PowerUp>    PowerUps;
	SpriteRenderer*         m_SpriteRenderer;
	GameState               m_eState;
	unsigned int            Lives;
	bool                    m_bKeys[1024];
	bool                    m_bKeysProcessed[1024];
	unsigned int            m_uiWidth, m_uiHeight;
};

#endif