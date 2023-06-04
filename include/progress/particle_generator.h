#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "game_object.h"


//�������ӵ�״̬
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ����Ч����
class ParticleGenerator
{
public:

    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    
	//������������
    void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    
	//��Ⱦ��������
    void Draw();

private:
	//��ʼ��������
    void init();
    
	// �ҵ���һ�����������Ӳ��ҷ�����������ֵ�� �������� Life <= 0.0f or 0
    unsigned int firstUnusedParticle();

    // ��������
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

private:
	std::vector<Particle> m_particles;
	unsigned int m_amount;
	Shader m_shader;
	Texture2D m_texture;
	unsigned int m_VAO;
};

#endif