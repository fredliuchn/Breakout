#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <irrklang/irrKlang.h>

#include <algorithm>

#include "progress/game.h"
#include "resource_manager.h"
#include "progress/ball_object.h"
#include "progress/particle_generator.h"
#include "progress/post_processor.h"
#include "text_renderer.h"
#include <sstream>

using namespace irrklang;

//��Чʱ��
GLfloat ShakeTime = 0.0f;

GameObject *Player;
BallObject *Ball;
ParticleGenerator *Particles;
PostProcessor     *Effects;
TextRenderer  *Text;
ISoundEngine      *SoundEngine = createIrrKlangDevice();
extern std::string m_workspace;

Game::Game(unsigned int width, unsigned int height) 
	: m_eState(GAME_ACTIVE), m_bKeys(), m_uiWidth(width), m_uiHeight(height),Lives(3)
{
}

Game::~Game()
{
	delete m_SpriteRenderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init()
{
	std::string strspritevs = m_workspace + "\\..\\resources\\shader\\sprite.vs";
	std::string strspritefs = m_workspace + "\\..\\resources\\shader\\sprite.fs";

	std::string strparticlevs = m_workspace + "\\..\\resources\\shader\\particle.vs";
	std::string strparticlefs = m_workspace + "\\..\\resources\\shader\\particle.fs";

	std::string strpostprocessingvs = m_workspace + "\\..\\resources\\shader\\post_processing.vs";
	std::string strpostprocessingfs = m_workspace + "\\..\\resources\\shader\\post_processing.fs";

	ResourceManager::LoadShader(strspritevs.c_str(), strspritefs.c_str(), nullptr, "sprite");
	ResourceManager::LoadShader(strparticlevs.c_str(), strparticlefs.c_str(), nullptr, "particle");
	ResourceManager::LoadShader(strpostprocessingvs.c_str(), strpostprocessingfs.c_str(), nullptr, "postprocessing");

	//�� �� �� �� �� Զ
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_uiWidth), static_cast<float>(m_uiHeight), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

	//��������
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\background.png", GL_TRUE, "background");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\particle.png", GL_TRUE, "particle");

	//����
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_sticky.png", true, "powerup_sticky");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture(m_workspace + "\\..\\resources\\textures\\powerup_passthrough.png", true, "powerup_passthrough");
	
	//����
	Text = new TextRenderer(m_uiWidth, m_uiHeight);
	Text->Load(m_workspace + "\\..\\resources\\fonts\\OCRAEXT.TTF", 24);
	//���عؿ�
	GameLevel onelevel;
	std::string stronelevel = m_workspace + "\\..\\resources\\levels\\one.lvl";
	onelevel.Load(stronelevel.c_str(), m_uiWidth, m_uiHeight*0.5);
	
	GameLevel twolevel;
	std::string strtwolevel = m_workspace + "\\..\\resources\\levels\\two.lvl";
	twolevel.Load(strtwolevel.c_str(), m_uiWidth, m_uiHeight*0.5);

	GameLevel threelevel;
	std::string strthreelevel = m_workspace + "\\..\\resources\\levels\\three.lvl";
	threelevel.Load(strthreelevel.c_str(), m_uiWidth, m_uiHeight*0.5);

	GameLevel fourlevel;
	std::string strfourlevel = m_workspace + "\\..\\resources\\levels\\four.lvl";
	fourlevel.Load(strfourlevel.c_str(), m_uiWidth, m_uiHeight*0.5);

	m_levels[0] = onelevel;
	m_levels[1] = twolevel;
	m_levels[2] = threelevel;
	m_levels[3] = fourlevel;

	m_level = 0;

	//���Ʊ���
	m_SpriteRenderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	//���õ���
	glm::vec2 playerPos = glm::vec2(m_uiWidth / 2 - PLAYER_SIZE.x / 2, m_uiHeight - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//����С��
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));

	//С��β��ľ���
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), m_uiWidth, m_uiHeight);

	std::string strbleep = m_workspace + "\\..\\resources\\audio\\bleep.mp3";
	SoundEngine->play2D(strbleep.c_str(), false);
}

void Game::ProcessInput(float dt)
{
	if (m_eState == GAME_MENU)
	{
		if (m_bKeys[GLFW_KEY_ENTER] && !m_bKeysProcessed[GLFW_KEY_ENTER])
		{
			m_eState = GAME_ACTIVE;
			m_bKeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (m_bKeys[GLFW_KEY_W] && !m_bKeysProcessed[GLFW_KEY_W])
		{
			m_level = (m_level + 1) % 4;
			m_bKeysProcessed[GLFW_KEY_W] = true;
		}
		if (m_bKeys[GLFW_KEY_S] && !m_bKeysProcessed[GLFW_KEY_S])
		{
			if (m_level > 0)
				--m_level;
			else
				m_level = 3;
			m_bKeysProcessed[GLFW_KEY_S] = true;
		}
	}
	if (m_eState == GAME_WIN)
	{
		if (m_bKeys[GLFW_KEY_ENTER])
		{
			m_bKeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			m_eState = GAME_MENU;
		}
	}
	if (m_eState == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;
		if (m_bKeys[GLFW_KEY_A])
		{
			if (Player->m_Position.x >= 0.0f)
			{
				Player->m_Position.x -= velocity;
				if (Ball->m_bStuck)
					Ball->m_Position.x -= velocity;
			}
		}
		if (m_bKeys[GLFW_KEY_D])
		{
			if (Player->m_Position.x <= m_uiWidth - Player->m_Size.x)
			{
				Player->m_Position.x += velocity;
				if (Ball->m_bStuck)
					Ball->m_Position.x += velocity;
			}
		}
		if (m_bKeys[GLFW_KEY_SPACE])
			Ball->m_bStuck = false;
	}
}

void Game::Update(float dt)
{
	//���ƶ�
	Ball->Move(dt, m_uiWidth);

	// �����ײ
	DoCollisions();

	//����С���β��λ��
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->m_fRadius / 2));

	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}
	//���µ���
	UpdatePowerUps(dt);
	// ���Ƿ�Ӵ��ײ��߽磿
	if (Ball->m_Position.y >= m_uiHeight) 
	{
		--Lives;
		if (Lives == 0)
		{
			ResetLevel();
			m_eState = GAME_MENU;
		}
		ResetPlayer();
	}
	//����Ƿ�Ӯ��
	if (m_eState == GAME_ACTIVE && m_levels[m_level].IsCompleted())
	{
		ResetLevel();
		ResetPlayer();
		Effects->Chaos = true;
		m_eState = GAME_WIN;
	}
}

void Game::Render()
{
	
	if (m_eState == GAME_ACTIVE)
	{
		Effects->BeginRender();

		//���Ʊ���
		m_SpriteRenderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(m_uiWidth, m_uiHeight), 0.0f);

		//���ƹؿ�
		m_levels[m_level].Draw(*m_SpriteRenderer);

		// ���Ƶ���
		Player->Draw(*m_SpriteRenderer);

		//���Ƶ���
		for (PowerUp &powerUp : PowerUps)
			if (!powerUp.m_Destroyed)
				powerUp.Draw(*m_SpriteRenderer);
		//����С���β��
		Particles->Draw();

		//������
		Ball->Draw(*m_SpriteRenderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());

		//�ı�
		std::stringstream ss;
		ss << Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
		//Text->RenderText("Level:" + std::to_string(m_level + 1), 5.0f, 55.0f, 1.0f);
	}
	if (m_eState == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, m_uiHeight / 2.0f, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, m_uiHeight / 2.0f + 20.0f, 0.75f);
	}
	if (m_eState == GAME_WIN)
	{
		Text->RenderText("You WON!!!", 320.0f, m_uiHeight / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, m_uiHeight / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
	Text->RenderText("Level:" + std::to_string(m_level + 1), 5.0f, 55.0f, 1.0f);
}
void ActivatePowerUp(PowerUp &powerUp);
bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::DoCollisions()
{
	for (GameObject &box : m_levels[m_level].m_Bricks)
	{
		if (!box.m_Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // if collision is true
			{
				//���ٲ��ǹ�̬������
				if (!box.m_IsSolid)
				{
					box.m_Destroyed = true;
					SpawnPowerUps(box);
				}
				else
				{   //����ǹ�̬�ļ�һ��������Ч��
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}
				std::string strbleep = m_workspace + "\\..\\resources\\audio\\bleep.mp3";
				SoundEngine->play2D(strbleep.c_str(), false);
				//��ײ����
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->m_bPassThrough && !box.m_IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
				{
					//ˮƽ������ײ
					if (dir == LEFT || dir == RIGHT)
					{
						//��תˮƽ�ٶ�
						Ball->m_Velocity.x = -Ball->m_Velocity.x;
						//�ض�λ
						float penetration = Ball->m_fRadius - std::abs(diff_vector.x);
						if (dir == LEFT)
							//��������
							Ball->m_Position.x += penetration;
						else
							//��������
							Ball->m_Position.x -= penetration;
					}
					//��ֱ������ײ
					else
					{
						//��ת��ֱ�ٶ�
						Ball->m_Velocity.y = -Ball->m_Velocity.y;
						//�ض�λ
						float penetration = Ball->m_fRadius - std::abs(diff_vector.y);
						if (dir == UP)
							//��������
							Ball->m_Position.y -= penetration;
						else
							//��������
							Ball->m_Position.y += penetration;
					}
				}
			}
		}
	}

	
	for (PowerUp &powerUp : PowerUps)
	{
		if (!powerUp.m_Destroyed)
		{
			//���ٵ���
			if (powerUp.m_Position.y >= m_uiHeight)
				powerUp.m_Destroyed = true;

			if (CheckCollision(*Player, powerUp))
			{	//�������
				ActivatePowerUp(powerUp);
				powerUp.m_Destroyed = true;
				powerUp.Activated = true;
				std::string strpowerup = m_workspace + "\\..\\resources\\audio\\powerup.wav";
				SoundEngine->play2D(strpowerup.c_str(), false);
			}
		}
	}

	//����ҵĵ�������ײ���
	Collision result = CheckCollision(*Ball, *Player);

	if (!Ball->m_bStuck && std::get<0>(result))
	{
		//�����ײ�������λ�ã���������ײ�������λ�øı��ٶ�
		//���㵲�������λ�õ�x����
		float centerBoard = Player->m_Position.x + Player->m_Size.x / 2.0f;
		//�������ĵ��������ĵľ���(������)
		float distance = (Ball->m_Position.x + Ball->m_fRadius) - centerBoard;
		//�������ĵ��������ĵľ���ռ�������峤�ȵİٷֱ�
		float percentage = distance / (Player->m_Size.x / 2.0f);
		// ���ݽ���ƶ�
		float strength = 2.0f;
		//֮����Ҫ�洢�ɵ��ٶ�����Ϊ����ֻ��������ٶ�ʸ����ˮƽ������ٶȲ���������y�ٶȲ��䡣
		//�⽫��ζ��ʸ���ĳ��Ȼ�����仯���������Ӱ���������ײ��������ı�Ե����ײ�������������и���(Ҳ��˸�ǿ)���ٶ�ʸ����
		//Ϊ���µ��ٶ�ʸ����������Ȼ����Ծ��ٶ�ʸ���ĳ��ȡ�����һ��������������ٶȽ�����һһ�µģ�������ײ����������ĸ��ط���
		glm::vec2 oldVelocity = Ball->m_Velocity;
		Ball->m_Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->m_Velocity = glm::normalize(Ball->m_Velocity) * glm::length(oldVelocity); 
		//��ֹС���ٶȹ��죬����ճ�����⣬�����ٶȷ���ָ��
		Ball->m_Velocity.y = -1.0f * abs(Ball->m_Velocity.y);

		//���ճ�Ե��߱����һ��������µ��ٶ�ʸ����ҲҪ����ճ������
		Ball->m_bStuck = Ball->m_bStuck;

		std::string strbleep = m_workspace + "\\..\\resources\\audio\\bleep.wav";
		SoundEngine->play2D(strbleep.c_str(), false);
	}
}

bool Game::CheckCollision(GameObject &one, GameObject &two)
{
	// x�᷽����ײ��
	bool collisionX = one.m_Position.x + one.m_Size.x >= two.m_Position.x &&
		two.m_Position.x + two.m_Size.x >= one.m_Position.x;
	// y�᷽����ײ��
	bool collisionY = one.m_Position.y + one.m_Size.y >= two.m_Position.y &&
		two.m_Position.y + two.m_Size.y >= one.m_Position.y;
	// ֻ��������������ײʱ����ײ
	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject & one, GameObject & two)
{
	// ��ȡԲ������ 
	glm::vec2 center(one.m_Position + one.m_fRadius);
	// ����AABB����Ϣ�����ġ���߳���
	glm::vec2 aabb_half_extents(two.m_Size.x / 2.0f, two.m_Size.y / 2.0f);
	glm::vec2 aabb_center(two.m_Position.x + aabb_half_extents.x, two.m_Position.y + aabb_half_extents.y);
	// ��ȡ�������ĵĲ�ʸ��
	glm::vec2 difference = center - aabb_center;
	//��ȡ��aabb�ϵĽ���
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center����clamped�����͵õ�����ײ���Ͼ���Բ����ĵ�closest
	glm::vec2 closest = aabb_center + clamped;
	// ���Բ��center�������closest��ʸ�����ж��Ƿ� length <= radius
	//difference�����ķ���Ҳ������ײ��������ĸ���
	difference = closest - center;

	if (glm::length(difference) < one.m_fRadius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
			break;
		}
	}
	return (Direction)best_match;
}

void Game::ResetLevel()
{
	if (m_level == 0)
	{
		GameLevel onelevel;
		std::string stronelevel = m_workspace + "\\..\\resources\\levels\\one.lvl";
		onelevel.Load(stronelevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels[0] = onelevel;
	}
	else if (m_level == 1)
	{
		GameLevel twolevel;
		std::string strtwolevel = m_workspace + "\\..\\resources\\levels\\two.lvl";
		twolevel.Load(strtwolevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels[1] = twolevel;
	}
	else if (m_level == 2)
	{
		GameLevel threelevel;
		std::string strthreelevel = m_workspace + "\\..\\resources\\levels\\three.lvl";
		threelevel.Load(strthreelevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels[2] = threelevel;
	}
	else if (m_level == 3)
	{
		GameLevel fourlevel;
		std::string strfourlevel = m_workspace + "\\..\\resources\\levels\\four.lvl";
		fourlevel.Load(strfourlevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels[3] = fourlevel;
	}

	Lives = 3;
}

void Game::ResetPlayer()
{
	// ����
	Player->m_Size = PLAYER_SIZE;
	Player->m_Position = glm::vec2(m_uiWidth / 2.0f - PLAYER_SIZE.x / 2.0f, m_uiHeight - PLAYER_SIZE.y);
	Ball->Reset(Player->m_Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
	//ʧ�����е���
	Effects->Chaos = Effects->Confuse = false;
	Ball->m_bPassThrough = Ball->m_bSticky = false;
	Player->m_Color = glm::vec3(1.0f);
	Ball->m_Color = glm::vec3(1.0f);
}

void Game::UpdatePowerUps(float dt)
{
	for (PowerUp &powerUp : PowerUps)
	{
		powerUp.m_Position += powerUp.m_Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				//����Ƴ�
				powerUp.Activated = false;
				if (powerUp.Type == "sticky")
				{
					// ����û������stickyЧ�����ڼ���״̬ʱ���ã�����ͬ��
					if (!IsOtherPowerUpActive(PowerUps, "sticky"))
					{
						Ball->m_bSticky = false;
						Player->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(PowerUps, "pass-through"))
					{
						Ball->m_bPassThrough = false;
						Ball->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(PowerUps, "confuse"))
					{
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(PowerUps, "chaos"))
					{
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.m_Destroyed && !powerUp.Activated; }
	), PowerUps.end());
}

bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}
void Game::SpawnPowerUps(GameObject &block)
{
	if (ShouldSpawn(75))
		PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.m_Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.m_Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.m_Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.m_Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15))
		PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.m_Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.m_Position, ResourceManager::GetTexture("powerup_chaos")));
}

void ActivatePowerUp(PowerUp &powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ball->m_Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->m_bSticky = true;
		Player->m_Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->m_bPassThrough = true;
		Ball->m_Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->m_Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true;
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}