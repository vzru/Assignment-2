// RU ZHANG 100421055 //
// Kyle Disante - 100617178 //

#define GLEW_STATIC
#include "glew/glew.h"
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "FreeImage.lib")

#include "GameObject.h"
#include "AnimationMath.h"
#include "ParticleEmitter.h"
#include "KeyframeController.h"
#include "PointHandle.h"

// Core Libraries (std::)
#include <iostream>
#include <string>
#include <math.h>
#include <vector>

// 3rd Party Libraries
#include <GLUT/freeglut.h>
#include <TTK/GraphicsUtils.h>
#include <TTK/Camera.h>
#include "imgui/imgui.h"

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame

// Window size
int windowWidth = 1600;
int windowHeight = 1000;

// Angle conversions
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

float deltaTime = 0.0f; // amount of time since last update (set every frame in timer callback)
int selectedEmitter = 0;

// Mouse position in pixels
glm::vec3 mousePosition; // x, y, 0
glm::vec3 mousePositionFlipped; // x, windowHeight - y, 0

GameObject gameObject;

std::vector<PointHandle> pointHandles;

KeyframeController<glm::vec3> keyframeController; // holds and updates our keyframe animation

//std::vector<ParticleEmitter> emitters;
static std::vector<ParticleEmitter*> emitters;

void InitializeScene()
{
	//ParticleEmitter* emitter = new ParticleEmitter;
	//// Physics properties
	//emitter->velocity0 = glm::vec3(0.0f, 0.0f, 0.0f);
	//emitter->velocity1 = glm::vec3(1.0f, 1.0f, 0.0f);
	//emitter->massRange = glm::vec2(0.5f, 1.0f);
	//emitter->applyGravity = glm::vec3(0.f, 9.8f, false);
	//
	//// Visual Properties
	//emitter->colour0 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//emitter->colour1 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	//emitter->lifeRange = glm::vec2(1.0f, 10.0f);
	//emitter->sizeRange = glm::vec2(15.0f, 25.0f);
	//emitter->emitterPosition = mousePositionFlipped;
	//emitter->emitterSize = glm::vec3(1.0f, 1.0f, false);
	//emitter->emissionDelay = 0.0f;
	//emitter->emissionRate = 100.0f;
	//emitter->cooldown = 0.0f;
	//emitter->gravity = 9.8f;
	//
	//
	//// Create the particles
	//emitter->initialize(100);
	//emitters.push_back(emitter);


	emitters.push_back(new ParticleEmitter);
	gameObject.setScale(50.0f); // when in 2D the rendering utilities use pixels as units
	gameObject.mesh = std::make_shared<QuadMesh>();
}

// These values are controlled by imgui
bool connectedEndpoints = false;
//bool applySeekingForce = true;
bool path = false;
int numParticles = 100;
//float seekingForceScale = 100.0f;
float minSeekingForceScale = -500.0f;
float maxSeekingForceScale = 500.0f;


// This is where we draw stuff
void DisplayCallbackFunction(void)
{
	// Set up scene
	TTK::Graphics::SetBackgroundColour(1.f, 1.f, 1.f);
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetCameraMode2D(windowWidth, windowHeight);

	//std::cout << windowHeight << ":" << windowWidth << std::endl;
	for (int i = 0; i < emitters.size(); i++)
	{
		// Apply forces on the particle system
		if (emitters[i]->applySeekingForce)
		{
			emitters[i]->applyForcesToParticleSystem();
		}
		// perform physics calculations for each particle
		if (emitters[i]->playing)
		{
			emitters[i]->update(deltaTime);
		}
		// draw the particles
		if (emitters[i]->show)
		{
			emitters[i]->draw();
		}
	}
	glm::vec3 interpolatedPosition = keyframeController.update(deltaTime);

	gameObject.setPosition(interpolatedPosition);
	gameObject.update(deltaTime);
	gameObject.draw();

	//// Draw lines between point handles
	//for (int i = pointHandles.size() - 1; i > 0; i--)
	//	TTK::Graphics::DrawLine(pointHandles[i].position, pointHandles[i - 1].position, 5.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	keyframeController.drawCatmull();

	// Draw point handles
	for (int i = 0; i < pointHandles.size(); i++)
		pointHandles[i].draw();


	// You must call this prior to using any imgui functions
	TTK::Graphics::StartUI(windowWidth, windowHeight);


	for (int i = 0; i < emitters.size(); i++) {
		ImGui::SameLine();
		ImGui::RadioButton(("Emit" + std::to_string(i)).c_str(), &selectedEmitter, i);
	}
	if (ImGui::Button("add")) {
		emitters.push_back(new ParticleEmitter());
		selectedEmitter = emitters.size() - 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("remove")) {
		if (emitters.size() > 1)
			emitters.erase(selectedEmitter + emitters.begin());

		if (selectedEmitter == emitters.size())
		{
			selectedEmitter -= 1;
		}
	}
	// Draw a simple label, this is the same as a "printf" call
	ImGui::Text("Particle Options. Number of particles = %d", emitters[selectedEmitter]->getNumParticles());

	ImGui::SliderFloat("Emission Rate", &emitters[selectedEmitter]->emissionRate, 1.0f, 100.f);

	ImGui::SliderInt("Particle Amount", &numParticles, 0, 500);
	if (ImGui::Button("Apply Particle Change"))
	{
		emitters[selectedEmitter]->initialize(numParticles);
	}

	ImGui::SliderFloat("Emitter X", &emitters[selectedEmitter]->emitterPosition.x, 0, windowWidth);
	ImGui::SliderFloat("Emitter Y", &emitters[selectedEmitter]->emitterPosition.y, 0, windowHeight);


	ImGui::SliderFloat("Life Range Min", &emitters[selectedEmitter]->lifeRange.x, 1.0f, emitters[selectedEmitter]->lifeRange.y);
	ImGui::SliderFloat("Life Range Max", &emitters[selectedEmitter]->lifeRange.y, emitters[selectedEmitter]->lifeRange.x, 100.0f);

	// Button, when button is clicked the code in the block is executed


	if (ImGui::Button("Toggle Box Emitter"))
	{
		std::cout << "Box Emitter Clicked." << std::endl;
		emitters[selectedEmitter]->emitterSize.z = !emitters[selectedEmitter]->emitterSize.z;
	}
	if (emitters[selectedEmitter]->emitterSize.z)
	{
		ImGui::SliderFloat("Box width", &emitters[selectedEmitter]->emitterSize.x, 1, 100);
		ImGui::SliderFloat("Box height", &emitters[selectedEmitter]->emitterSize.y, 1, 100);
	}

	// Color control
	// Tip: You can click and drag the numbers in the UI to change them
	ImGui::ColorEdit4("Start Color", &emitters[selectedEmitter]->colour0[0]);
	ImGui::ColorEdit4("End Color", &emitters[selectedEmitter]->colour1[0]);
	if (ImGui::Button("Changing Color"))
	{
		std::cout << "Color clicked." << std::endl;
		emitters[selectedEmitter]->interpolateColour = !emitters[selectedEmitter]->interpolateColour;
	}

	for (int i = 0; i < emitters[selectedEmitter]->forces.size(); i++) {
		ImGui::SameLine();
		ImGui::RadioButton(("Force" + std::to_string(i)).c_str(), &emitters[selectedEmitter]->selectedForce, i);
	}
	if (ImGui::Button("add force")) {
		emitters[selectedEmitter]->addForce();
	}
	ImGui::SameLine();
	if (ImGui::Button("remove force")) {
		emitters[selectedEmitter]->removeForce(emitters[selectedEmitter]->selectedForce);
	}

	if (ImGui::Button("Toggle Steering Force"))
	{
		std::cout << "Steering clicked." << std::endl;
		emitters[selectedEmitter]->applySeekingForce = !emitters[selectedEmitter]->applySeekingForce;
	}
	if (emitters[selectedEmitter]->applySeekingForce)
	{
		ImGui::SliderFloat("Steering Force", &emitters[selectedEmitter]->forces[emitters[selectedEmitter]->selectedForce].z, minSeekingForceScale, maxSeekingForceScale);
		ImGui::SliderFloat("Target X", &emitters[selectedEmitter]->forces[emitters[selectedEmitter]->selectedForce].x, 0, windowWidth);
		ImGui::SliderFloat("Target Y", &emitters[selectedEmitter]->forces[emitters[selectedEmitter]->selectedForce].y, 0, windowHeight);
		if (ImGui::Button("Toggle Magnet Force"))
		{
			std::cout << "Magnet clicked." << std::endl;
			emitters[selectedEmitter]->forces[emitters[selectedEmitter]->selectedForce].w = !emitters[selectedEmitter]->forces[emitters[selectedEmitter]->selectedForce].w;
		}
	}


	if (ImGui::Button("Toggle Gravity"))
	{
		std::cout << "Gravity clicked." << std::endl;
		emitters[selectedEmitter]->applyGravity.z = !emitters[selectedEmitter]->applyGravity.z;
	}
	if (emitters[selectedEmitter]->applyGravity.z)
		ImGui::SliderFloat("Gravity", &emitters[selectedEmitter]->gravity, 1.0f, 100.f);

	ImGui::DragFloatRange2("Initial Velocity in X", &emitters[selectedEmitter]->velocity0.x, &emitters[selectedEmitter]->velocity1.x, 0.25f, -100.0f, 100.0f, "Min: %.1f %", "Max: %.1f %");
	ImGui::DragFloatRange2("Initial Velocity in Y", &emitters[selectedEmitter]->velocity0.y, &emitters[selectedEmitter]->velocity1.y, 0.25f, -100.0f, 100.0f, "Min: %.1f %", "Max: %.1f %");

	if (ImGui::Button("Create Path"))
	{
		std::cout << "Path clicked." << std::endl;
		path = !path;
	}
	if (ImGui::Button("Speed Control"))
	{
		std::cout << "Speed Control clicked." << std::endl;
		keyframeController.doesSpeedControl = !keyframeController.doesSpeedControl;
	}
	if (keyframeController.doesSpeedControl)
		ImGui::SliderFloat("Speed", &keyframeController.curvePlaySpeed, 0.0f, 10.0f);

	if (ImGui::Button("Toggle Emitter Pause"))
	{
		std::cout << "Pause clicked." << std::endl;
		emitters[selectedEmitter]->playing = !emitters[selectedEmitter]->playing;
	}
	// imgui has TONS of UI functions
	// Uncomment these two lines if you want to see a full imgui demo
	// This is about the best documentation available for imgui.
	// If you see some functionality in the demo you would like to use, go into 
	// the ShowTestWindow function and pick it apart. 
	// Note: keyboard input is currently broken, if you get it working please let the TA know :)
	//static bool drawWindow = true;
	//ImGui::ShowTestWindow(&drawWindow);

	// You must call this once you are done doing UI stuff
	// This is what actually draws the ui on screen
	TTK::Graphics::EndUI();

	// Swap buffers
	// This is how we tell the program to put the things we just drew on the screen
	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = true;
	io.AddInputCharacter((int)key);

	switch (key)
	{
	case 27: // the escape key
		glutExit();
		break;

	case 'q': // the 'q' key
	case 'Q':
		// ...
		break;
	}
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;
	io.AddInputCharacter((int)key);

	switch (key)
	{
	case ' ':
		keyframeController.doesSpeedControl = !keyframeController.doesSpeedControl;
		break;
	case '+':
		keyframeController.curvePlaySpeed += 0.25f;
		break;
	case '-':
		keyframeController.curvePlaySpeed -= 0.25f;
		break;
	default:
		break;
	}
	switch (key)
	{
	default:
		break;
	}
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
void TimerCallbackFunction(int value)
{
	// Calculate the amount of time since the last frame
	static int elapsedTimeAtLastTick = 0;
	int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = (float)totalElapsedTime - elapsedTimeAtLastTick;
	deltaTime /= 1000.0f;
	elapsedTimeAtLastTick = totalElapsedTime;

	// Re-trigger the display event
	glutPostRedisplay();

	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;
}

// This is called when a mouse button is clicked
void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	ImGui::GetIO().MouseDown[0] = !state;

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;

	//ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
	//
	//mousePositionFlipped.x = x;
	//mousePositionFlipped.y = windowHeight - y;
	//
	//if (!ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), 1))
	//{
	//}
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (!path)
			emitters[selectedEmitter]->emitterPosition = mousePositionFlipped;
		else if (path)
		{
			switch (state)
			{
			case GLUT_DOWN:
				switch (button)
				{
					// On mouse left button down
				case GLUT_LEFT_BUTTON:
				{
					int numHandles = pointHandles.size();
					PointHandle newHandle(25.0f, mousePositionFlipped, std::to_string(numHandles));
					pointHandles.push_back(newHandle);
					keyframeController.addKey(mousePositionFlipped);
				}
				default:
					break;
				}

			case GLUT_UP:
				switch (button)
				{
					// on mouse left button up
				case GLUT_LEFT_BUTTON:
					// recalculate lookup table using 4 samples per segment
					keyframeController.calculateLookupTable(4);
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
	}
}

void SpecialInputCallbackFunction(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		// ...
		break;
	case GLUT_KEY_DOWN:
		// ...
		break;
	case GLUT_KEY_LEFT:
		// ...
		break;
	case GLUT_KEY_RIGHT:
		// ...
		break;
	}
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;

	//if (!ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), 1))
	//{
	//}

	if (!ImGui::GetIO().WantCaptureMouse) {
		emitters[selectedEmitter]->emitterPosition = mousePositionFlipped;
		if (path)
		{
			keyframeController.calculateLookupTable(4);
			for (unsigned int i = 0; i < pointHandles.size(); i++)
			{
				if (pointHandles[i].isInside(mousePositionFlipped))
				{
					pointHandles[i].position = mousePositionFlipped;
					keyframeController.setKey(i, mousePositionFlipped);

					break;
				}
			}
		}
	}
}

// Called when the mouse is moved inside the window
void MousePassiveMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;


	if (!ImGui::GetIO().WantCaptureMouse) {
		//	emitters[selectedEmitter]->emitterPosition = mousePositionFlipped;
	}
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	/* initialize the window and OpenGL properly */

	// Request an OpenGL 4.4 compatibility
	// A compatibility context is needed to use the provided rendering utilities 
	// which are written in OpenGL 1.1
	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Tutorial");

	//Init GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "TTK::InitializeTTK Error: GLEW failed to init" << std::endl;
	}
	printf("OpenGL version: %s, GLSL version: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutSpecialFunc(SpecialInputCallbackFunction);

	// Init GL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* Start Game Loop */
	deltaTime = (float)glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	InitializeScene();

	// Init IMGUI
	TTK::Graphics::InitImGUI();

	glutMainLoop();

	return 0;
}