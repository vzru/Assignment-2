// RU ZHANG 100421055 //
// Kyle Disante - 100617178 //

#include <iostream> // for std::cout
#include <glm\gtc\random.hpp> // for glm::linearRand
#include <TTK\GraphicsUtils.h> // for drawing utilities

#include "AnimationMath.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
	: m_pParticles(nullptr),
	m_pNumParticles(0),
	playing(true),
	interpolateColour(false)
{
}

ParticleEmitter::~ParticleEmitter()
{
	freeMemory();
}

void ParticleEmitter::initialize(unsigned int numParticles)
{
	freeMemory(); // destroy any existing particles

	if (numParticles > 0)
	{
		m_pParticles = new Particle[numParticles];
		memset(m_pParticles, 0, sizeof(Particle) * numParticles);
		m_pNumParticles = numParticles;
	}
}

void ParticleEmitter::freeMemory()
{
	if (m_pParticles) // if not a null pointer
	{
		delete[] m_pParticles;
		m_pParticles = nullptr;
		m_pNumParticles = 0;
	}
}

void ParticleEmitter::update(float dt)
{
	if (m_pParticles && playing) // make sure memory is initialized and system is playing
	{
		// loop through each particle
		Particle* particle = m_pParticles;
		for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
		{
			if (particle->life <= 0) // if particle has no life remaining
			{
				if (cooldown <= 0)
				{
					//std::cout << "Spawned " << particle << std::endl;
					cooldown = 1.0f / emissionRate;
					// Respawn particle
					// Note: we are not freeing memory, we are "Recycling" the particles
					particle->acceleration = glm::vec3(0.0f);
					float randomTval = glm::linearRand(0.0f, 1.0f);
					particle->colour = Math::lerp(colour0, colour1, randomTval);
					randomTval = glm::linearRand(0.0f, 1.0f);
					particle->life = Math::lerp(lifeRange.x, lifeRange.y, randomTval);
					randomTval = glm::linearRand(0.0f, 1.0f);
					particle->mass = Math::lerp(massRange.x, massRange.y, randomTval);
					if (emitterSize.z)
					{
						randomTval = glm::linearRand(0.0f, 1.0f);
						particle->position.x = Math::lerp(emitterPosition.x - emitterSize.x, emitterPosition.x + emitterSize.x, randomTval);
						randomTval = glm::linearRand(0.0f, 1.0f);
						particle->position.y = Math::lerp(emitterPosition.y - emitterSize.y, emitterPosition.y + emitterSize.y, randomTval);
					}
					else
					{
						particle->position = emitterPosition;
					}
					randomTval = glm::linearRand(0.0f, 1.0f);
					particle->size = Math::lerp(sizeRange.x, sizeRange.y, randomTval);
					//velocity0.x = velocity0.y = initialVelo.x;
					//velocity1.x = velocity1.y = initialVelo.y;
					randomTval = glm::linearRand(0.0f, 1.0f);
					particle->velocity = Math::lerp(velocity0, velocity1, randomTval);
				}
				// Update physics

			}
			else
			{
				// Update acceleration
				particle->acceleration = particle->force / particle->mass;
				if (applyGravity.z)
				{
					applyGravity.y = -gravity;
					particle->velocity = particle->velocity + (particle->acceleration * dt) + (applyGravity * dt);
				}
				else
				{
					particle->velocity = particle->velocity + (particle->acceleration * dt);

				}
				particle->position = particle->position + particle->velocity * dt;

				// We've applied the force, let's remove it so it does not get applied next frame
				particle->force = glm::vec3(0.0f);

				// Decrease particle life
				particle->life -= dt;

				// Update visual properties
				if (interpolateColour)
				{
					// calculate t value
					float tVal = Math::invLerp(particle->life, lifeRange.x, lifeRange.y);
					particle->colour = Math::lerp(colour1, colour0, tVal);
				}
			}
		}
		cooldown -= dt;
		//std::cout << dt << "; \t" << cooldown << std::endl;

	}
}

void ParticleEmitter::draw()
{
	// Draw the emitter position
	// Note: not necessary
	TTK::Graphics::DrawPoint(emitterPosition, 1.0f, glm::vec4(1.0f));

	Particle* p = m_pParticles;
	for (int i = 0; i < m_pNumParticles; ++i, ++p)
	{
		if (p->life >= 0.0f) // if particle is alive, draw it
		{
			TTK::Graphics::DrawCube(p->position, p->size, p->colour);
			//TTK::Graphics::DrawPoint(p->position, p->size, p->colour);
			//TTK::Graphics::DrawTeapot(p->position, p->size, p->colour); // low fps alert!! use with low particle count
			// You can draw anything!
			// ...but you should stick to points in this framework since it uses GL 1.0
		}
	}
}

void ParticleEmitter::applyForceToParticle(unsigned int idx, glm::vec3 force)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::applyForceToParticle ERROR: idx " << idx << "out of range!" << std::endl;
		return;
	}

	m_pParticles[idx].force = force;
}

//void ParticleEmitter::setNumParticles(int particle)
//{
//	m_pNumParticles = particle;
//}

glm::vec3 ParticleEmitter::getParticlePosition(unsigned int idx)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
		return glm::vec3();
	}

	return m_pParticles[idx].position;
}
