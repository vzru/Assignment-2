#pragma once
#include <string>
#include <vector>
#include <GLM\glm\vec2.hpp>
#include <GLM\glm\vec4.hpp>

namespace Help {
	void drawUI();

	template <class T>
	T radioTree(std::vector<std::string> names, int* select);
	template <class T>
	T radioTree(std::vector<std::string> names, int* select, char* comboName, int* comboSelect, char* comboString);

	template<class T>
	bool listInterface(char* name, std::vector<T*>* ts, int* select);

	template<class T>
	void radioList(char* name, std::vector<T*>* ts, int* select);

	template<class T>
	bool inRange(std::vector<T*>* ts, int* select);

	void toggleButton(bool* state, char* onState, char* offState);

	template<class T>
	void addButton(char* name, std::vector<T*>* ts);
	template<class T>
	void addButton(char* name, std::vector<T*>* ts, int* select);

	template<class T>
	void duplicateButton(char* name, std::vector<T*>* ts, int* select);

	template<class T>
	void removeButton(char* name, std::vector<T*>* ts, int* select);

	void rangeSlider(char* name, int* val);
	void rangeSlider(char* name, float* val);
	void rangeSlider(char* name, float* val, bool small);
	void rangeSlider(char* name, float* val, float* var);
	void rangeSlider(char* name, float* val, float* var, bool positive);
	void rangeSlider(char* name, glm::vec2* val);
	void rangeSlider(char* name, glm::vec2* val, glm::vec2* var);

	float randLerp(float val, float var);
	glm::vec2 randLerp(glm::vec2 val, glm::vec2 var);
	glm::vec4 randLerp(glm::vec4 val, glm::vec4 var);
}

enum class Prop {
	MAIN,
	INITIAL,
	START,
	END,
	BEHAVIOR,
	PATH
};
enum class EmitProp {
	POSITION,
	VELOCITY,
	GRAVITY
};
enum class PartProp {
	COLOR,
	SIZE,
	MASS
};

namespace Bound {
	namespace Float {
		static const float zero = 0.f;
		static const float small = 0.01f;
		static const float one = 1.f;
		static const float ten = 10.f;
		static const float hundred = 100.f;
		static const float thousand = 1000.f;
	};
	namespace Int {
		static const int zero = 0;
		static const int one = 1;
		static const int ten = 10;
		static const int hundred = 100;
		static const int thousand = 1000;
	};
};