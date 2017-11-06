
#include "Helper.h"
#include "imgui\imgui.h"
#include <glm\gtc\random.hpp> // for glm::linearRand
#include "AnimationMath.h"
#include "ParticleEmitter.h"

static int selected[3] = { 0, 0, 0 };
static std::vector<Emitter*> emitters;

void Help::drawUI() {
	if (ImGui::Button("Reset")) {
		// call a method to free all the memory
		// call a method to initialize all the memory
	}
	ImGui::SameLine();

	if (Help::listInterface("Emit", &emitters, &selected[0])) {
		ImGui::SameLine();
		Help::toggleButton(&emitters[selected[0]]->show, "Hide", "Show");
		ImGui::SameLine();
		Help::toggleButton(&emitters[selected[0]]->play, "Pause", "Play");
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			// implement a pop up filename window to save emitters[selected[0]] to (TODO)
		}
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			// implement a pop up filename window to load into emitters[selected[0]] (TODO)
		}

		ImGui::PushItemWidth(200.f);

		switch (Help::radioTree<Prop>({ "Main", "Initial", "Start", "End", "Behavior", "Path" }, &selected[1])) {
		case Prop::MAIN:
			Help::rangeSlider("Lifetime", &emitters[selected[0]]->life, &emitters[selected[0]]->vinLife, true);

			Help::rangeSlider("Frequency", &emitters[selected[0]]->freq, &emitters[selected[0]]->vinFreq, true);

			Help::rangeSlider("Rate", &emitters[selected[0]]->rate);
			ImGui::SameLine();
			ImGui::SliderFloat("Duration", &emitters[selected[0]]->duration, -Bound::Float::one, Bound::Float::ten);
			Help::rangeSlider("Maximum number of particles", &emitters[selected[0]]->maxParts);
			break;
		case Prop::INITIAL:
			switch (Help::radioTree<EmitProp>({ "position", "velocity", "gravity" }, &selected[2],
				"shape", &emitters[selected[0]]->shape[2], "rectangle\0circle\0\0")) {
			case EmitProp::POSITION:
				Help::rangeSlider("postiion", &emitters[selected[0]]->pos, &emitters[selected[0]]->vinPos);
				break;
			case EmitProp::VELOCITY:
				Help::rangeSlider("velocity", &emitters[selected[0]]->vel, &emitters[selected[0]]->vinVel);
				break;
			case EmitProp::GRAVITY:
				Help::rangeSlider("gravity", &emitters[selected[0]]->grav, &emitters[selected[0]]->vinGrav);
				break;
			}
			break;
		case Prop::START: case Prop::END:
			switch (Help::radioTree<PartProp>({ "color", "size", "mass" }, &selected[2],
				"shape", &emitters[selected[0]]->shape[selected[1] - 1], "rectangle\0circle\0\0")) {
			case PartProp::COLOR:
				ImGui::PushItemWidth(400.f);
				ImGui::ColorEdit4("color", &emitters[selected[0]]->color[selected[1] - 1][0]);
				ImGui::ColorEdit4("+-", &emitters[selected[0]]->vinColor[selected[1] - 1][0]);
				break;
			case PartProp::SIZE:
				Help::rangeSlider("size", &emitters[selected[0]]->size[selected[1] - 1], &emitters[selected[0]]->vinSize[selected[1] - 1]);
				break;
			case PartProp::MASS:
				Help::rangeSlider("mass", &emitters[selected[0]]->mass[selected[1] - 1], &emitters[selected[0]]->vinMass[selected[1] - 1]);
				break;
			}
			break;
		case Prop::BEHAVIOR:
			if (Help::listInterface("Behave", &emitters[selected[0]]->behaves, &selected[2])) {
				ImGui::SameLine();
				ImGui::PushItemWidth(100.f);
				ImGui::Combo("type", &emitters[selected[0]]->behaves[selected[2]]->type, "seek\0flee\0attract\0repel\0\0");
				ImGui::PopItemWidth();

				Help::rangeSlider("position", &emitters[selected[0]]->behaves[selected[2]]->pos);

				Help::rangeSlider("weight", &emitters[selected[0]]->behaves[selected[2]]->weight, true);
				ImGui::SameLine();
				ImGui::SliderFloat("mass", &emitters[selected[0]]->behaves[selected[2]]->weight, Bound::Float::zero, Bound::Float::hundred);

				ImGui::Text("Specific behaviors can be temporarily disabled by setting their weight to zero.");
			}
			break;
		case Prop::PATH:
			Help::rangeSlider("weight", &emitters[selected[0]]->pathWeight, true);
			ImGui::SameLine();
			ImGui::Combo("type", &emitters[selected[0]]->pathType, "lerp\0bezier\0catmull-rom\0\0");
			for (int i = 0; i < emitters[selected[0]]->path.size(); i++) {
				Help::removeButton("path", &emitters[selected[0]]->path, &i);
				ImGui::SameLine();
				Help::rangeSlider("position", emitters[selected[0]]->path[i]);
			}
			Help::addButton("path", &emitters[selected[0]]->path);
			ImGui::SameLine();
			ImGui::Text("%d", emitters[selected[0]]->path.size());
			break;
		}
	}
}

template<class T>
bool Help::listInterface(char* name, std::vector<T*>* ts, int* select) {
	Help::radioList(name, ts, select);

	if (inRange(ts, select)) {
		ImGui::Indent();
		ImGui::Text("%s%d", name, *select);
		ImGui::SameLine();
		Help::removeButton(name, ts, select);
		ImGui::SameLine();
		Help::duplicateButton(name, ts, select);
		return inRange(ts, select);
	}
	return false;
}

template <class T>
T Help::radioTree(std::vector<std::string> names, int* select) {
	for (int i = 0; i < names.size(); i++) {
		ImGui::RadioButton(names[i].c_str(), select, i);
		ImGui::SameLine();
	}
	ImGui::Text("");
	ImGui::Indent();
	return (T)*select;
}
template <class T>
T Help::radioTree(std::vector<std::string> names, int* select, char* comboName, int* comboSelect, char* comboString) {
	for (int i = 0; i < names.size(); i++) {
		ImGui::RadioButton(names[i].c_str(), select, i);
		ImGui::SameLine();
	}
	ImGui::PushItemWidth(100.f);
	ImGui::Combo(comboName, comboSelect, comboString);
	ImGui::PopItemWidth();
	ImGui::Indent();
	return (T)*select;
}

template<class T>
void Help::radioList(char* name, std::vector<T*>* ts, int* select) {
	for (int i = 0; i < ts->size(); i++) {
		ImGui::RadioButton(std::string(name + std::to_string(i)).c_str(), select, i);
		ImGui::SameLine();
	}
	Help::addButton(name, ts, select);
}

template<class T>
bool Help::inRange(std::vector<T*>* ts, int* select) {
	return (*select >= 0 && *select < ts->size());
}

void Help::toggleButton(bool* state, char* onState, char* offState) {
	if (ImGui::Button(*state ? onState : offState)) {
		*state = !*state;
	}
}

template<class T>
void Help::addButton(char* name, std::vector<T*>* ts) {
	ImGui::PushID(name);
	if (ImGui::Button("+")) {
		ts->push_back(new T());
	}
	ImGui::PopID();
}
template<class T>
void Help::addButton(char* name, std::vector<T*>* ts, int* select) {
	ImGui::PushID(name);
	if (ImGui::Button("+")) {
		*select = ts->size();
		ts->push_back(new T());
	}
	ImGui::PopID();
}

template<class T>
void Help::duplicateButton(char* name, std::vector<T*>* ts, int* select) {
	ImGui::PushID(name);
	if (ImGui::Button("Duplicate")) {
		ts->push_back(new T(*(*ts)[*select]));
		*select = ts->size() - 1;
	}
	ImGui::PopID();
}

template<class T>
void Help::removeButton(char* name, std::vector<T*>* ts, int* select) {
	ImGui::PushID(name);
	if (ImGui::Button("Remove")) {
		delete (*ts)[*select];
		ts->erase(*select + ts->begin());
		if (*select == ts->size()) {
			(*select)--;
		}
	}
	ImGui::PopID();
}

void Help::rangeSlider(char* name, float* val, float* var) {
	rangeSlider(name, val);
	ImGui::SameLine();
	ImGui::PushID(name);
	ImGui::SliderFloat("+-", var, Bound::Float::zero, Bound::Float::hundred);
	ImGui::PopID();
}
void Help::rangeSlider(char* name, float* val, float* var, bool positive) {
	if (positive) {
		ImGui::SliderFloat(name, val, Bound::Float::small, Bound::Float::ten);
		ImGui::SameLine();
		ImGui::PushID(name);
		ImGui::SliderFloat("+-", var, Bound::Float::zero, Bound::Float::ten);
		ImGui::PopID();
	}
}
void Help::rangeSlider(char* name, float* val) {
	ImGui::SliderFloat(name, val, -Bound::Float::hundred, Bound::Float::hundred);
}
void Help::rangeSlider(char* name, float* val, bool small) {
	if (small) {
		ImGui::SliderFloat(name, val, -Bound::Float::one, Bound::Float::one);
	}
}
void Help::rangeSlider(char* name, int* val) {
	ImGui::SliderInt(name, val, Bound::Int::one, Bound::Int::thousand);
}
void Help::rangeSlider(char* name, glm::vec2* val) {
	ImGui::PushID(name);
	Help::rangeSlider("x", &val->x);
	ImGui::SameLine();
	Help::rangeSlider("y", &val->y);
	ImGui::PopID();
}
void Help::rangeSlider(char* name, glm::vec2* val, glm::vec2* var) {
	ImGui::PushID(name);
	Help::rangeSlider("x", &val->x, &var->x);
	Help::rangeSlider("y", &val->y, &var->y);
	ImGui::PopID();
}

float Help::randLerp(float val, float var) {
	return Math::lerp(val - var, val + var, glm::linearRand(0.0f, 1.0f));
}
glm::vec2 Help::randLerp(glm::vec2 val, glm::vec2 var) {
	return {
		Help::randLerp(val.x, var.x), randLerp(val.y, var.y)
	};
}
glm::vec4 Help::randLerp(glm::vec4 val, glm::vec4 var) {
	return {
		Help::randLerp(val.r, var.r), Help::randLerp(val.g, var.g),
		Help::randLerp(val.b, var.b), Help::randLerp(val.a, var.a)
	};
}