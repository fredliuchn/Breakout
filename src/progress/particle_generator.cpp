#include "progress/particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : m_shader(shader), m_texture(texture), m_amount(amount)
{
    init();
}

void ParticleGenerator::Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset)
{
    //增加新的粒子
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        this->respawnParticle(m_particles[unusedParticle], object, offset);
    }
    //更新所有粒子的状态
    for (unsigned int i = 0; i < m_amount; ++i)
    {
        Particle &p = m_particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * dt; 
            p.Color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::Draw()
{
    //这种组合使自身颜色得到了增加
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_shader.Use();
    for (Particle particle : m_particles)
    {
        if (particle.Life > 0.0f)
        {
            m_shader.SetVector2f("offset", particle.Position);
            m_shader.SetVector4f("color", particle.Color);
			
			glActiveTexture(GL_TEXTURE0);
			m_texture.Bind();
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    for (unsigned int i = 0; i < m_amount; ++i)
		m_particles.push_back(Particle());
}

//存储上一次消亡的粒子的索引值
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    //从上一次消亡开始遍历
	for (unsigned int i = lastUsedParticle; i < m_amount; ++i) 
	{
		if (m_particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
    //简单查询
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
	{
        if (m_particles[i].Life <= 0.0f)
		{
            lastUsedParticle = i;
            return i;
        }
    }
    //所有粒子都消亡了
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
	//得到一个[ -5.0, 4.9]中的一个随机值
    float random = ((rand() % 100) - 50) / 10.0f;
	//得到一个[ 0.5, 0.99]中的一个随机颜色值
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.m_Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.m_Velocity * 0.1f;
}