// Kyle Disante - 100617178 //
#pragma once

#include <glm/glm.hpp>
#include <vector>

class Particle;
class Behavior;
class Emitter;

class Particle {
public:
	Particle();
	~Particle();

	void update(Emitter* e, float dt);
	void init(Emitter* e);

	glm::vec2 pos;
	glm::vec2 vel;
	glm::vec2 acc;
	glm::vec2 force;
	glm::vec2 size, targetSize;
	glm::vec4 color, targetColor;

	float mass, targetMass;
	float life, totalLife;
};

class Behavior {
public:
	Behavior();
	~Behavior();

	glm::vec2 update(Particle* p, float dt);

	int type = 0;
	glm::vec2 pos{ 0, 0 };
	float weight = 1.f;
	float mass = 10.f;
};

// vin is short for variation in
// 0/1 are short for start/end
class Emitter {
public:
	Emitter();
	~Emitter();

	void update(float dt);

	bool show = true;
	bool play = true;

	// particle distribution properties
	float freq, vinFreq;
	float life, vinLife;
	float duration;
	int rate, maxParts;

	// emitter/initial properties
	glm::vec2 pos{ 0.f, 0.f };
	glm::vec2 vinPos{ 0.f, 0.f };
	glm::vec2 vel{ 10.f, 10.f };
	glm::vec2 vinVel{ 1.f, 1.f };
	glm::vec2 grav{ 0.f, 9.81f };
	glm::vec2 vinGrav{ 0.1f, 0.1f };

	// start/end properties to lerp inbetween
	int shape[3]{ 0, 0, 1 }; // start/end/emitter
	glm::vec4 color[2]{ { 1.f, 0.f, 0.f, 1.f },{ 0.f, 0.f, 1.f, 1.f } };
	glm::vec4 vinColor[2]{ { 0.f, 0.1f, 0.1f, 0.1f },{ 0.1f, 0.1f, 0.f, 0.1f } };
	glm::vec2 size[2]{ { 5.f, 5.f },{ 5.f, 5.f } };
	glm::vec2 vinSize[2]{ { 1.f, 1.f },{ 1.f, 1.f } };
	float mass[2]{ 10.f, 10.f };
	float vinMass[2]{ 0.1f, 0.1f };

	int pathType = 0;
	float pathWeight = 1.f;

	std::vector<Particle*> parts{};
	std::vector<Behavior*> behaves{};
	std::vector<glm::vec2*> path{};
};