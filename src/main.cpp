// RU ZHANG 100421055 //

#define GLEW_STATIC
#include "glew/glew.h"
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "FreeImage.lib")

#include "GameObject.h"
#include "AnimationMath.h"
#include "ParticleEmitter.h"
#include "KeyframeController.h"
#include "PointHandle.h"
#include "Helper.h"

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

// Mouse position in pixels
glm::vec3 mousePosition; // x, y, 0
glm::vec3 mousePositionFlipped; // x, windowHeight - y, 0

GameObject gameObject;

std::vector<PointHandle> pointHandles;
KeyframeController<glm::vec3> keyframeController; // holds and updates our keyframe animation
int selected[3] = { 0, 0, 0 };
std::vector<Emitter*> emitters;

void InitializeScene()
{
	// Create the emitter
	Emitter* emit = new Emitter();

	emit->duration = -1.f;
	emit->life = 1.1f;
	emit->vinLife = 0.4f;
	emit->rate = 250.f;
	emit->maxParts = 500.f;

	emit->shape[2] = 1;
	emit->pos = { 0, 0 };
	emit->vinPos = { 0, 0 };
	emit->vel = { 0, 0 };
	emit->vinVel = { 0, 0 };
	emit->grav = { 0, 0 };

	emit->show = true;
	emit->behaves = {};
	emitters.push_back(emit);
}

// This is where we draw stuff
void DisplayCallbackFunction(void)
{
	// Set up scene
	TTK::Graphics::SetBackgroundColour(0.5f, 0.5f, 0.5f);
	TTK::Graphics::ClearScreen();
	TTK::Graphics::SetCameraMode2D(windowWidth, windowHeight);

	// You must call this prior to using any imgui functions
	TTK::Graphics::StartUI(windowWidth, windowHeight);

	Help::drawUI();
	
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
	ImGuiIO& io = ImGui::GetIO();

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;

	if (button == GLUT_LEFT_BUTTON) io.MouseDown[0] = !state;
	else if (button == GLUT_MIDDLE_BUTTON) io.MouseDown[1] = !state;
	else if (button == GLUT_RIGHT_BUTTON) io.MouseDown[2] = !state;
	if (state = GLUT_DOWN)
		if (button == 3) io.MouseWheel = 1.f;
		else if (button == 4) io.MouseWheel = -1.f;
		else io.MouseWheel = 0.f;
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
}

// Called when the mouse is moved inside the window
void MousePassiveMotionCallbackFunction(int x, int y)
{
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = windowHeight - y;

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