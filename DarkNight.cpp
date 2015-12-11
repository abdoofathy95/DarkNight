#include "Camera.h"
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <iostream>
#  include <GLUT/glut.h>
#  include "fontstash.h"
#else
#  define GLEW_STATIC
#  include <array>
#  include <GL\glew.h>
#  include <glut.h>
#  include "Shader.h"
#  include "fontstash.h"
#  include "MainMenuEntry.h"
#endif
extern "C" {
	struct sth_stash* sth_create(int cachew, int cacheh);
	void sth_begin_draw(struct sth_stash* stash);
	void sth_draw_text(struct sth_stash* stash,
		int idx, float size,
		float x, float y, const char* string, float* dx);
	void sth_end_draw(struct sth_stash* stash);
	int sth_add_font(struct sth_stash* stash, const char* path);
	void sth_delete(struct sth_stash* stash);
}
// FUNCTIONS DEFENITIONS
int configureShader();
void renderFlashlight();
void renderString(char *text, float x, float y, float r, float g, float b, int fontSize);
void drawDot(float x, float y, float z);
void keyboard(unsigned char key, int x, int y);
void mouse(int x, int y);
void fpsTimer(int value); // FPS (CONTROL THE RATE POST DISPLAY IS CALLED ( CURRENTLY 60)
void drawHUD();
void drawMainMenu();
void drawMenuEntry(char *text,int x, int y, float r, float g, float b,int count);
void mouseMenu(int button, int state, int x, int y);
void initMenuEntries();

////////////////////////GAMEPLAY CONSTANTS////////////////////////////////////
float mouseSensitivity = 1.1;
float characterSpeed = 0.01;
///////////////////// DISPLAY CONSTANTS/////////////////////////////
const float WINDOW_WIDTH = 1366, WINDOW_HEIGHT = 768; //
const float FPS = 60.0;
float oldMouseX = WINDOW_WIDTH / 2, oldMouseY = WINDOW_HEIGHT / 2;
const int FONT_SIZE = 100, DEFAULT_ENTRY_SIZE = 50;
char *FILE_PATH = "./FreeFonts/bloody.ttf";
Camera camera = Camera(0.5, 0.04, 0.5, 0.4, 0.05, 0.4);
bool gameStarted, gameLost, gameWon;
int shaderId,programId; // SHADER
const float LIGHT_SPOT_INNER_CONE = 0.5f;
const float LIGHT_SPOT_OUTER_CONE = 15.0f;
// Calculate the cos cutoff angles for use in the spot light shader.
float g_cosInnerCone = cosf(LIGHT_SPOT_INNER_CONE * (3.14159265358979323846f / 180.0f));
float g_cosOuterCone = cosf(LIGHT_SPOT_OUTER_CONE * (3.14159265358979323846f / 180.0f));
float g_lightPos[4];
///////////////// MENU CONSTANTS & Attributes////////////////
MainMenuEntry start;
MainMenuEntry settings;
MainMenuEntry help;
MainMenuEntry quit;
MainMenuEntry menuEntries[4]; // array holding all 4 buttons


void drawWall(double thickness) {
	glPushMatrix();
	glTranslated(0.5 * thickness, 0, 0.5 * thickness);
	glScaled(thickness, 0.001, thickness);
	glutSolidCube(1);
	glPopMatrix();
}

int configureShader() {
	Shader shader;
	programId = glCreateProgram();
	shader.loadAndCompileVertexShader("./Shaders/flashlight.vs",programId);
	shader.loadAndCompileFragShader("./Shaders/flashlight.frag", programId);
	glLinkProgram(programId);
	return programId;
}

void setupLights() {
	GLfloat ambient[4] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
	GLfloat diffuse[4] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
	GLfloat specular[4] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
	GLfloat emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat shininess = 51.2f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.2);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);

}
void setupCamera() {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, WINDOW_WIDTH / WINDOW_HEIGHT, 0.001, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void Display() {

	if (gameLost) {
		// show game over and restart button
	}

	if (gameWon) {
		// show you are out or whatever
	}

	if (!gameStarted) {
		drawMainMenu();
	}
	else {
		glUseProgram(shaderId);
		renderFlashlight();
		glutSetCursor(GLUT_CURSOR_NONE);
		setupCamera();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(0.752941, 0.752941, 0.752941);
		drawWall(100);
		glPushMatrix();
		glRotated(90, 0, 0, 1.0);
		drawWall(2);
		glPopMatrix();
	}
	glFlush();
}

void Keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'w':
		camera.moveForward(characterSpeed);
		break;
	case 's':
		camera.moveBackward(characterSpeed);
		break;
	case 'a':
		camera.moveLeft(characterSpeed);
		break;
	case 'd':
		camera.moveRight(characterSpeed);
		break;
	case 27:
		exit(0);
		break;
	}

	glutPostRedisplay();
}

void mouse(int x, int y) { // handle the main menu hover and ingame motion
	if (!gameStarted) {
		if (x >= menuEntries[0].x &&
			x <= menuEntries[0].x + 100 &&
			y <= menuEntries[0].y &&
			y >= menuEntries[0].y - 40) { // first entry
			menuEntries[0].hover = true;
			menuEntries[1].hover = false;
			menuEntries[2].hover = false;
			menuEntries[3].hover = false;
		}
		else if(x >= menuEntries[1].x &&
			x < menuEntries[1].x + 180 &&
			y >= menuEntries[1].y + 110 &&
			y < menuEntries[1].y + 150) { // second entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = true;
			menuEntries[2].hover = false;
			menuEntries[3].hover = false;
		}
		else if (x >= menuEntries[2].x &&
			x <= menuEntries[2].x + 100 &&
			y >= menuEntries[2].y + 250 &&
			y <= menuEntries[2].y + 290) { // thirds entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = true;
			menuEntries[3].hover = false;
		}
		else if (x >= menuEntries[3].x &&
			x < menuEntries[3].x + 80 &&
			y >= menuEntries[3].y + 385 &&
			y < menuEntries[3].y +430) {// forth entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = false;
			menuEntries[3].hover = true;
		}
		else {
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = false;
			menuEntries[3].hover = false;
		}
	}
	else {
		//camera.rotateX((oldMouseY-y)*mouseSensitivity);
		if (x <= 0 || x >= WINDOW_WIDTH - 10) {
			glutWarpPointer(WINDOW_WIDTH / 2, y);
		}
		camera.rotateY((oldMouseX - x)*mouseSensitivity);
		oldMouseX = x;
		//oldMouseY = y;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Dark Night (Egyptian edition)");
	glutDisplayFunc(Display);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutFullScreen();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2); // Position mouse at middle of screen
	glEnable(GL_DEPTH_TEST); // ENABLE PERSPECTIVE CAMERA
	glEnable(GL_LIGHTING);	// ENABLE LIGHT 
	glEnable(GL_LIGHT0);
	setupLights();
	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_NORMALIZE); 	// ENABLE NORMALIZE OF VECTORS AFTER ANY OPERATION 
	glEnable(GL_COLOR_MATERIAL); // ENABLE COLOR MATERIAL
	glShadeModel(GL_SMOOTH); // ENABLE SHADING
	glDepthMask(GL_TRUE);

	// REGISTER CALLBACKS
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(mouseMenu);
	glutPassiveMotionFunc(mouse);
	// INITIALIZERS
	initMenuEntries();
	glewInit();
	shaderId = configureShader();
	glutMainLoop();
}

void drawDot(float x, float y, float z) {
	glPointSize(20);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(x, y, z);
	glEnd();
}

void drawMainMenu() {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	/* HUD ELEMENTs DRAWING*/
	renderString("AYAM SODA", WINDOW_WIDTH / 3.8, WINDOW_HEIGHT / 1.3, 0.75, 0, 0, 150);
	drawMenuEntry("Play", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 2, 0.5, 0, 0,1);
	drawMenuEntry("Settings", WINDOW_WIDTH / 2.35, WINDOW_HEIGHT / 2.5, 0.5, 0, 0,2);
	drawMenuEntry("Help", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 3.2, 0.5, 0, 0,3);
	drawMenuEntry("Exit", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 4.5, 0.5, 0, 0,4);
	
	/*END OF ELEMENTS */
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	// Making sure we can render 3d again
	glDepthMask(GL_TRUE);  // disable writes to Z-Buffer
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();
}

////////////////////// FONT HANDLING //////////////////////////////

void renderString(char *text, float x, float y, float r, float g, float b, int fontSize)
{
	struct sth_stash* stash = sth_create(512, 512);
	/* load truetype font */
	int bloody = sth_add_font(stash, FILE_PATH);
	/* draw text during your OpenGL render loop */
	glColor3f(r, g, b);
	sth_begin_draw(stash);
	/* position: (x, y); */
	sth_draw_text(stash, bloody, fontSize, x, y, text, &x);
	/* now, the float x contains the x position of the next char */
	sth_end_draw(stash);
	sth_delete(stash);
}

void drawMenuEntry(char *text,int x,int y,float r,float g,float b,int count) {
	menuEntries[count-1].x = x;
	menuEntries[count-1].y = y;
	if(menuEntries[count-1].hover == true)
		renderString(text, x, y, 1, g, b, DEFAULT_ENTRY_SIZE);
	else
		renderString(text,x, y,r,g,b,DEFAULT_ENTRY_SIZE);
}

void mouseMenu(int button,int state,int x, int y) { // handle the main menu clicks
	if (!gameStarted) {
		if (x >= menuEntries[0].x &&
			x <= menuEntries[0].x + 100 &&
			y <= menuEntries[0].y &&
			y >= menuEntries[0].y - 40) { // first entry
			gameStarted = true;
		}
		else if (x >= menuEntries[1].x &&
			x < menuEntries[1].x + 180 &&
			y >= menuEntries[1].y + 110 &&
			y < menuEntries[1].y + 150) { // second entry
			// show Settings
		}
		else if (x >= menuEntries[2].x &&
			x <= menuEntries[2].x + 100 &&
			y >= menuEntries[2].y + 250 &&
			y <= menuEntries[2].y + 290) { // thirds entry
			// show help
		}
		else if (x >= menuEntries[3].x &&
			x < menuEntries[3].x + 80 &&
			y >= menuEntries[3].y + 385 &&
			y < menuEntries[3].y + 430) {// forth entry
			exit(0);
		}
	}
}

void initMenuEntries() {
	menuEntries[0] = start;
	menuEntries[1] = settings;
	menuEntries[2] = help;
	menuEntries[3] = quit;
}

void renderFlashlight() {
	g_lightPos[0] = camera.eye.x;
	g_lightPos[1] = camera.eye.y;
	g_lightPos[2] = camera.eye.z;
	g_lightPos[3] = 1.0f;
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, LIGHT_SPOT_OUTER_CONE);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);
	glUniform1i(glGetUniformLocation(programId, "colorMap"), 0);
	glUniform1f(glGetUniformLocation(programId, "cosInnerCone"), g_cosInnerCone);
	glUniform1f(glGetUniformLocation(programId, "cosOuterCone"), g_cosOuterCone);
}