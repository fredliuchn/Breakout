#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "progress/game.h"
#include "resource_manager.h"
#include "progress/ball_object.h"
#include "progress/particle_generator.h"
#include "progress/post_processor.h"

//初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const GLfloat BALL_RADIUS = 12.5f;
//特效时间
GLfloat ShakeTime = 0.0f;

GameObject *Player;
BallObject *Ball;
ParticleGenerator *Particles;
PostProcessor     *Effects;
extern std::string m_workspace;

Game::Game(unsigned int width, unsigned int height) 
	: m_eState(GAME_ACTIVE), m_bKeys(), m_uiWidth(width), m_uiHeight(height)
{
}

Game::~Game()
{
	delete m_SpriteRenderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
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

	//左 右 下 上 近 远
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_uiWidth), static_cast<float>(m_uiHeight), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

	//加载纹理
	std::string strbackground = m_workspace + "\\..\\resources\\textures\\background.png";
	ResourceManager::LoadTexture(strbackground.c_str(), GL_TRUE, "background");
	std::string strtes = m_workspace + "\\..\\resources\\textures\\awesomeface.png";
	ResourceManager::LoadTexture(strtes.c_str(), GL_TRUE, "face");
	std::string strblock = m_workspace + "\\..\\resources\\textures\\block.png";
	ResourceManager::LoadTexture(strblock.c_str(), GL_FALSE, "block");
	std::string strblock_solid = m_workspace + "\\..\\resources\\textures\\block_solid.png";
	ResourceManager::LoadTexture(strblock_solid.c_str(), GL_FALSE, "block_solid");
	std::string strpaddle = m_workspace + "\\..\\resources\\textures\\paddle.png";
	ResourceManager::LoadTexture(strpaddle.c_str(), GL_TRUE, "paddle");
	std::string strparticle = m_workspace + "\\..\\resources\\textures\\particle.png";
	ResourceManager::LoadTexture(strparticle.c_str(), GL_TRUE, "particle");

	//加载关卡
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

	m_levels["onelevel"] = onelevel;
	m_levels["twolevel"] = twolevel;
	m_levels["threelevel"] = threelevel;
	m_levels["fourlevel"] = fourlevel;

	m_strlevel = "onelevel";

	//绘制背景
	m_SpriteRenderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	//设置挡板
	glm::vec2 playerPos = glm::vec2(m_uiWidth / 2 - PLAYER_SIZE.x / 2, m_uiHeight - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//设置小球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));

	//小球尾焰的精灵
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), m_uiWidth, m_uiHeight);
}

void Game::ProcessInput(float dt)
{
	if (m_eState == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// 移动玩家挡板
		if (m_bKeys[GLFW_KEY_A])
		{
			if (Player->m_Position.x >= 0)
				Player->m_Position.x -= velocity;
			if (Ball->m_bStuck)
				Ball->m_Position.x -= velocity;
		}
		if (m_bKeys[GLFW_KEY_D])
		{
			if (Player->m_Position.x <= this->m_uiWidth - Player->m_Size.x)
				Player->m_Position.x += velocity;
			if (Ball->m_bStuck)
				Ball->m_Position.x += velocity;
		}
		if (m_bKeys[GLFW_KEY_SPACE])
			Ball->m_bStuck = false;
	}
}

void Game::Update(float dt)
{
	//球移动
	Ball->Move(dt, m_uiWidth);

	// 检测碰撞
	DoCollisions();

	//更新小球的尾焰位置
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->m_fRadius / 2));

	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}

	// 球是否接触底部边界？
	if (Ball->m_Position.y >= m_uiHeight) 
	{
		ResetLevel();
		ResetPlayer();
		//m_eState = GAME_MENU;
	}
	//检查是否赢了
	if (m_eState == GAME_ACTIVE && m_levels[m_strlevel].IsCompleted())
	{
		this->ResetLevel();
		this->ResetPlayer();
		//Effects->Chaos = true;
		//m_eState = GAME_WIN;
	}
}

void Game::Render()
{
	if (m_eState == GAME_ACTIVE)
	{
		Effects->BeginRender();

		//绘制背景
		m_SpriteRenderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(m_uiWidth, m_uiHeight), 0.0f);

		//绘制关卡
		m_levels[m_strlevel].Draw(*m_SpriteRenderer);

		// 绘制挡板
		Player->Draw(*m_SpriteRenderer);

		//绘制小球的尾焰
		Particles->Draw();

		//绘制球
		Ball->Draw(*m_SpriteRenderer);

		Effects->EndRender();
		// Render postprocessing quad
		Effects->Render(glfwGetTime());
	}
	//m_SpriteRenderer->DrawSprite(ResourceManager::GetTexture("face"),
	//	glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Game::DoCollisions()
{
	for (GameObject &box : m_levels[m_strlevel].m_Bricks)
	{
		if (!box.m_Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // if collision is true
			{
				//销毁不是固态的物体
				if (!box.m_IsSolid)
				{
					box.m_Destroyed = true;
					//this->SpawnPowerUps(box);
					//SoundEngine->play2D(FileSystem::getPath("resources/audio/bleep.mp3").c_str(), false);
				}
				else
				{   //如果是固态的加一个颤抖的效果
					ShakeTime = 0.05f;
					Effects->Shake = true;
					//SoundEngine->play2D(FileSystem::getPath("resources/audio/bleep.mp3").c_str(), false);
				}
				//碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->m_bPassThrough && !box.m_IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
				{
					//水平方向碰撞
					if (dir == LEFT || dir == RIGHT)
					{
						//反转水平速度
						Ball->m_Velocity.x = -Ball->m_Velocity.x;
						//重定位
						float penetration = Ball->m_fRadius - std::abs(diff_vector.x);
						if (dir == LEFT)
							//将球右移
							Ball->m_Position.x += penetration;
						else
							//将球左移
							Ball->m_Position.x -= penetration;
					}
					//垂直方向碰撞
					else
					{
						//反转垂直速度
						Ball->m_Velocity.y = -Ball->m_Velocity.y;
						//重定位
						float penetration = Ball->m_fRadius - std::abs(diff_vector.y);
						if (dir == UP)
							//将球上移
							Ball->m_Position.y -= penetration;
						else
							//将球下移
							Ball->m_Position.y += penetration;
					}
				}
			}
		}
	}

	// also check collisions on PowerUps and if so, activate them
	//for (PowerUp &powerUp : this->PowerUps)
	//{
	//	if (!powerUp.Destroyed)
	//	{
	//		// first check if powerup passed bottom edge, if so: keep as inactive and destroy
	//		if (powerUp.Position.y >= this->Height)
	//			powerUp.Destroyed = true;

	//		if (CheckCollision(*Player, powerUp))
	//		{	// collided with player, now activate powerup
	//			ActivatePowerUp(powerUp);
	//			powerUp.Destroyed = true;
	//			powerUp.Activated = true;
	//			SoundEngine->play2D(FileSystem::getPath("resources/audio/powerup.wav").c_str(), false);
	//		}
	//	}
	//}

	//与玩家的挡板做碰撞检测
	Collision result = CheckCollision(*Ball, *Player);

	if (!Ball->m_bStuck && std::get<0>(result))
	{
		//检查它撞击挡板的位置，并根据它撞击挡板的位置改变速度
		//计算挡板的中心位置点x坐标
		float centerBoard = Player->m_Position.x + Player->m_Size.x / 2.0f;
		//计算球心到挡板中心的距离(有正负)
		float distance = (Ball->m_Position.x + Ball->m_fRadius) - centerBoard;
		//计算球心到挡板中心的距离占整个挡板长度的百分比
		float percentage = distance / (Player->m_Size.x / 2.0f);
		// 依据结果移动
		float strength = 2.0f;
		//之所以要存储旧的速度是因为我们只更新球的速度矢量中水平方向的速度并保持它的y速度不变。
		//这将意味着矢量的长度会持续变化，其产生的影响是如果球撞击到挡板的边缘则会比撞击到挡板中心有更大(也因此更强)的速度矢量。
		//为此新的速度矢量会正交化然后乘以旧速度矢量的长度。这样一来，球的力量和速度将总是一一致的，无论它撞击到挡板的哪个地方。
		glm::vec2 oldVelocity = Ball->m_Velocity;
		Ball->m_Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->m_Velocity = glm::normalize(Ball->m_Velocity) * glm::length(oldVelocity); 
		//防止小球速度过快，出现粘板问题，所以速度方向指定
		Ball->m_Velocity.y = -1.0f * abs(Ball->m_Velocity.y);

		//如果粘性道具被激活，一旦计算出新的速度矢量，也要将球粘到球拍
		Ball->m_bStuck = Ball->m_bStuck;

		//SoundEngine->play2D(FileSystem::getPath("resources/audio/bleep.wav").c_str(), false);
	}
}

bool Game::CheckCollision(GameObject &one, GameObject &two)
{
	// x轴方向碰撞？
	bool collisionX = one.m_Position.x + one.m_Size.x >= two.m_Position.x &&
		two.m_Position.x + two.m_Size.x >= one.m_Position.x;
	// y轴方向碰撞？
	bool collisionY = one.m_Position.y + one.m_Size.y >= two.m_Position.y &&
		two.m_Position.y + two.m_Size.y >= one.m_Position.y;
	// 只有两个轴向都有碰撞时才碰撞
	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject & one, GameObject & two)
{
	// 获取圆的中心 
	glm::vec2 center(one.m_Position + one.m_fRadius);
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(two.m_Size.x / 2.0f, two.m_Size.y / 2.0f);
	glm::vec2 aabb_center(two.m_Position.x + aabb_half_extents.x, two.m_Position.y + aabb_half_extents.y);
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	//获取在aabb上的交点
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	//difference向量的方向也代表着撞击方块的哪个面
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
	if (m_strlevel == "onelevel")
	{
		GameLevel onelevel;
		std::string stronelevel = m_workspace + "\\..\\resources\\levels\\one.lvl";
		onelevel.Load(stronelevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels["onelevel"] = onelevel;
	}
	else if (m_strlevel == "twolevel")
	{
		GameLevel twolevel;
		std::string strtwolevel = m_workspace + "\\..\\resources\\levels\\two.lvl";
		twolevel.Load(strtwolevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels["twolevel"] = twolevel;
	}
	else if (m_strlevel == "threelevel")
	{
		GameLevel threelevel;
		std::string strthreelevel = m_workspace + "\\..\\resources\\levels\\three.lvl";
		threelevel.Load(strthreelevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels["threelevel"] = threelevel;
	}
	else if (m_strlevel == "fourlevel")
	{
		GameLevel fourlevel;
		std::string strfourlevel = m_workspace + "\\..\\resources\\levels\\four.lvl";
		fourlevel.Load(strfourlevel.c_str(), m_uiWidth, m_uiHeight*0.5);
		m_levels["fourlevel"] = fourlevel;
	}

	//this->Lives = 3;
}

void Game::ResetPlayer()
{
	// reset player/ball stats
	Player->m_Size = PLAYER_SIZE;
	Player->m_Position = glm::vec2(this->m_uiWidth / 2.0f - PLAYER_SIZE.x / 2.0f, this->m_uiHeight - PLAYER_SIZE.y);
	Ball->Reset(Player->m_Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
	// also disable all active powerups
	Effects->Chaos = Effects->Confuse = false;
	Ball->m_bPassThrough = Ball->m_bSticky = false;
	Player->m_Color = glm::vec3(1.0f);
	Ball->m_Color = glm::vec3(1.0f);
}