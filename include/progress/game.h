#ifndef GAME_H
#define GAME_H

#include "SpriteRenderer.h"
#include "game_level.h"
#include "ball_object.h"

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

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

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
	std::map<std::string, GameLevel> m_levels;
	std::string m_strlevel;
	SpriteRenderer*         m_SpriteRenderer;
	GameState               m_eState;
	bool                    m_bKeys[1024];
	unsigned int            m_uiWidth, m_uiHeight;
};

#endif