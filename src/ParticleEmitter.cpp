// Kyle Disante - 100617178 //

#include <iostream> // for std::cout
#include <glm\gtc\random.hpp> // for glm::linearRand
#include <TTK\GraphicsUtils.h> // for drawing utilities

#include "AnimationMath.h"
#include "ParticleEmitter.h"
#include "Helper.h"

Particle::Particle() {}
Particle::~Particle() {}

Behavior::Behavior() {}
Behavior::~Behavior() {}

Emitter::Emitter() {}
Emitter::~Emitter() {}

glm::vec2 Behavior::update(Particle* p, float dt) {
	glm::vec2 fVec, fDir, force;
	float fScale = 100.f;
	switch (type) {
	case 0:
		fVec = pos - p->pos;
		break;
	case 1:
		fVec = p->pos - pos;
		break;
	case 2:
		break;
	case 3:
		break;
	}
	fDir = glm::normalize(fVec);
	force = fDir * fScale * weight;
	return force + p->force;
}

void Emitter::update(float dt) {
	if (play)
		for (auto part : parts)
			part->update(this, dt);
}

void Particle::update(Emitter* e, float dt) {
	if (life == 0) {
		init(e);
	}
	for (auto b : e->behaves)
		force = b->update(this, dt);
	acc = force / mass;
	pos += vel * dt;
	vel += acc * dt;
	life -= dt;
}

void Particle::init(Emitter* e) {
	// physics properties
	pos = Help::randLerp(e->pos, e->vinPos);
	vel = Help::randLerp(e->vel, e->vinVel);
	acc = Help::randLerp(e->grav, e->vinGrav);
	// properties which get lerped
	size = Help::randLerp(e->size[0], e->vinSize[0]);
	targetSize = Help::randLerp(e->size[1], e->vinSize[1]);
	mass = Help::randLerp(e->mass[0], e->vinMass[0]);
	targetMass = Help::randLerp(e->mass[1], e->vinMass[1]);
	color = Help::randLerp(e->color[0], e->vinColor[0]);
	targetColor = Help::randLerp(e->color[1], e->vinColor[1]);
	// t = life / totalLife
	totalLife = life = e->life;
	// helper values
	force = mass * acc;
}