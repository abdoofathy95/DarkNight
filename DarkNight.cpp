using namespace std;
#include "Camera.h"
#include <iostream>
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
#  include "SoundManager.h"
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

void updateCamera();
int configureShader();
void renderFlashlight();
void renderString(char *text, float x, float y, float r, float g, float b, int fontSize);
void keyboard(unsigned char key, int x, int y);
void mouse(int x, int y);
void fpsTimer(int value); // FPS (CONTROL THE RATE POST DISPLAY IS CALLED ( CURRENTLY 60)
void drawHUD();
void drawAdrenalineBar();
void fillAdrenalineBar(float value);
void handleAdrenaline(int value);
void drawMainMenu();
void drawMenuEntry(char *text, int x, int y, float r, float g, float b, int count);
void mouseMenu(int button, int state, int x, int y);
void initMenuEntries();

////////////////////////GAMEPLAY CONSTANTS////////////////////////////////////
float mouseSensitivity = 1.1;
float characterSpeed = 0.07;
float adrenalineLevel = 0.0f;
float adrenalineIncValue = 1; // how much the adrenaline level rise each second
float adrenalineIncTime = 5000; //5 seconds
const int MAX_ADRENALINE = 200;
///////////////////// DISPLAY CONSTANTS/////////////////////////////
const float WINDOW_WIDTH = 1366, WINDOW_HEIGHT = 768; //
const float FPS = 60.0;
float oldMouseX = WINDOW_WIDTH / 2, oldMouseY = WINDOW_HEIGHT / 2;
const int FONT_SIZE = 100, DEFAULT_ENTRY_SIZE = 50;
char *FILE_PATH = "./FreeFonts/bloody.ttf";
Camera camera;
bool gameStarted, gameLost, gameWon;
int shaderId, programId; // SHADER
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
							  //////////////// SOUND MANAGER ///////////////////
SoundManager sManager;
bool mainMusicStop = true, gameThemeStop = true, hoverSound = false;

void drawWall(float startX, float startZ, bool horizontal, float length, float thickness) {
	glPushMatrix();
	if (horizontal) {
		glTranslatef(startX + length / 2, 0.5, startZ + thickness / 2);
		glRotatef(-90, 0, 1, 0);
	}
	else {
		glTranslatef(startX + thickness / 2, 0.5, startZ + length / 2);
	}
	glScalef(thickness, 1, length);
	glutSolidCube(1);
	glPopMatrix();
}

int configureShader() {
	Shader shader;
	programId = glCreateProgram();
	shader.loadAndCompileVertexShader("./Shaders/flashlight.vs", programId);
	shader.loadAndCompileFragShader("./Shaders/flashlight.frag", programId);
	glLinkProgram(programId);
	return programId;
}

void setupLights() {
	GLfloat ambient[4] = { 0.59225f, 0.59225f, 0.59225f, 1.0f };
	GLfloat diffuse[4] = { 0.50754f, 0.50754f, 0.50754f, 0.0f };
	GLfloat specular[4] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
	GLfloat emission[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, WINDOW_WIDTH / WINDOW_HEIGHT, 0.001, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void updateCamera() {
	camera.look();
}

static void drawCoordinates() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
	glPopMatrix();
}

void Display() {

	if (gameLost) {
		// show game over and restart button
	}

	else if (gameWon) {
		// show you are out or whatever
	}

	else if (!gameStarted) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawMainMenu();
		if (mainMusicStop) { // called only once At the main menu
			mainMusicStop = false;
			sManager.playMainMenuMusic();
			sManager.stopBreathFastSound();
			glutTimerFunc(adrenalineIncTime, handleAdrenaline, adrenalineIncValue);
		}
	}
	else {
		if (!mainMusicStop) { // should enter once at the begining of the app (after clicking start)
			sManager.stopMainMenuMusic(); // stop main menu music
			sManager.playThemeMusic();// start gameplay theme music
			sManager.playBreathFastSound();
			mainMusicStop = true;
		}
		if (adrenalineLevel >= MAX_ADRENALINE) gameLost = true;
		glUseProgram(shaderId);
		renderFlashlight();
		drawAdrenalineBar();
		glutSetCursor(GLUT_CURSOR_NONE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawHUD();
		updateCamera();
		/*glColor3f(0.752941, 0.752941, 0.752941);
		drawWall(100);
		glPushMatrix();
		glRotated(90, 0, 0, 1.0);
		drawWall(2);*/
		//drawCoordinates();
		drawWall(0, 0, true, 5, 0.05);
		drawWall(0, 0, false, 15, 0.05);
		drawWall(5, 0, false, 15, 0.05);
		drawWall(-15, 15, true, 15, 0.05);
		drawWall(5, 15, true, 15, 0.05);
		drawWall(-15, 0, false, 15, 0.05);
		drawWall(-30, 0, true, 15, 0.05);


		drawWall(-15, 20, true, 30, 0.05);
		drawWall(-15, 35, true, 30, 0.05);
		drawWall(-15, 20, false, 15, 0.05);
		drawWall(15, 20, false, 15, 0.05);

		drawWall(-25, 5, true, 5, 0.05);
		drawWall(-25, 20, true, 5, 0.05);
		drawWall(-25, 5, false, 15, 0.05);
		drawWall(-20, 5, false, 15, 0.05);

		drawWall(-25, 25, true, 5, 0.05);
		drawWall(-25, 55, true, 5, 0.05);
		drawWall(-25, 25, false, 30, 0.05);
		drawWall(-20, 25, false, 30, 0.05);

		drawWall(-15, 55, true, 30, 0.05);
		drawWall(-15, 40, true, 30, 0.05);
		drawWall(-15, 40, false, 15, 0.05);
		drawWall(15, 40, false, 15, 0.05);

		drawWall(-30, 60, true, 50, 0.05);
		drawWall(20, 15, false, 45, 0.05);
		drawWall(-30, 0, false, 60, 0.05);
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
			if (!hoverSound) {
				sManager.playHoverSound();
				hoverSound = true;
			}
		}
		else if (x >= menuEntries[1].x &&
			x < menuEntries[1].x + 180 &&
			y >= menuEntries[1].y + 110 &&
			y < menuEntries[1].y + 150) { // second entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = true;
			menuEntries[2].hover = false;
			menuEntries[3].hover = false;
			if (!hoverSound) {
				sManager.playHoverSound();
				hoverSound = true;
			}
		}
		else if (x >= menuEntries[2].x &&
			x <= menuEntries[2].x + 100 &&
			y >= menuEntries[2].y + 250 &&
			y <= menuEntries[2].y + 290) { // thirds entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = true;
			menuEntries[3].hover = false;
			if (!hoverSound) {
				sManager.playHoverSound();
				hoverSound = true;
			}
		}
		else if (x >= menuEntries[3].x &&
			x < menuEntries[3].x + 80 &&
			y >= menuEntries[3].y + 385 &&
			y < menuEntries[3].y + 430) {// forth entry
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = false;
			menuEntries[3].hover = true;
			if (!hoverSound) {
				sManager.playHoverSound();
				hoverSound = true;
			}
		}
		else {
			menuEntries[0].hover = false;
			menuEntries[1].hover = false;
			menuEntries[2].hover = false;
			menuEntries[3].hover = false;
			hoverSound = false;
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

void initializeSectors() {
	camera.forwardSector[1] = 2;
	camera.rightSector[1] = 0;
	camera.backwardSector[1] = 0;
	camera.leftSector[1] = 0;
	camera.forward2[1] = 15;
	camera.right2[1] = 0;
	camera.backward2[1] = 0;
	camera.left2[1] = 5;

	camera.forwardSector[2] = 0;
	camera.rightSector[2] = 3;
	camera.backwardSector[2] = 1;
	camera.leftSector[2] = 27;
	camera.forward2[2] = 20;
	camera.right2[2] = 0;
	camera.backward2[2] = 15;
	camera.left2[2] = 5;

	camera.forwardSector[3] = 0;
	camera.rightSector[3] = 4;
	camera.backwardSector[3] = 0;
	camera.leftSector[3] = 2;
	camera.forward2[3] = 20;
	camera.right2[3] = -15;
	camera.backward2[3] = 15;
	camera.left2[3] = 0;

	camera.forwardSector[4] = 18;
	camera.rightSector[4] = 0;
	camera.backwardSector[4] = 5;
	camera.leftSector[4] = 3;
	camera.forward2[4] = 20;
	camera.right2[4] = -20;
	camera.backward2[4] = 15;
	camera.left2[4] = -15;

	camera.forwardSector[5] = 4;
	camera.rightSector[5] = 0;
	camera.backwardSector[5] = 6;
	camera.leftSector[5] = 0;
	camera.forward2[5] = 15;
	camera.right2[5] = -20;
	camera.backward2[5] = 5;
	camera.left2[5] = -15;

	camera.forwardSector[6] = 5;
	camera.rightSector[6] = 7;
	camera.backwardSector[6] = 0;
	camera.leftSector[6] = 0;
	camera.forward2[6] = 5;
	camera.right2[6] = -20;
	camera.backward2[6] = 0;
	camera.left2[6] = -15;

	camera.forwardSector[7] = 0;
	camera.rightSector[7] = 8;
	camera.backwardSector[7] = 0;
	camera.leftSector[7] = 6;
	camera.forward2[7] = 5;
	camera.right2[7] = -25;
	camera.backward2[7] = 0;
	camera.left2[7] = -20;

	camera.forwardSector[8] = 9;
	camera.rightSector[8] = 0;
	camera.backwardSector[8] = 0;
	camera.leftSector[8] = 7;
	camera.forward2[8] = 5;
	camera.right2[8] = -30;
	camera.backward2[8] = 0;
	camera.left2[8] = -25;

	camera.forwardSector[9] = 10;
	camera.rightSector[9] = 0;
	camera.backwardSector[9] = 8;
	camera.leftSector[9] = 0;
	camera.forward2[9] = 20;
	camera.right2[9] = -30;
	camera.backward2[9] = 5;
	camera.left2[9] = -25;

	camera.forwardSector[10] = 11;
	camera.rightSector[10] = 0;
	camera.backwardSector[10] = 9;
	camera.leftSector[10] = 20;
	camera.forward2[10] = 25;
	camera.right2[10] = -30;
	camera.backward2[10] = 20;
	camera.left2[10] = -25;

	camera.forwardSector[11] = 12;
	camera.rightSector[11] = 0;
	camera.backwardSector[11] = 10;
	camera.leftSector[11] = 0;
	camera.forward2[11] = 55;
	camera.right2[11] = -30;
	camera.backward2[11] = 25;
	camera.left2[11] = -25;

	camera.forwardSector[12] = 0;
	camera.rightSector[12] = 0;
	camera.backwardSector[12] = 11;
	camera.leftSector[12] = 13;
	camera.forward2[12] = 60;
	camera.right2[12] = -30;
	camera.backward2[12] = 55;
	camera.left2[12] = -25;

	camera.forwardSector[13] = 0;
	camera.rightSector[13] = 12;
	camera.backwardSector[13] = 0;
	camera.leftSector[13] = 14;
	camera.forward2[13] = 60;
	camera.right2[13] = -25;
	camera.backward2[13] = 55;
	camera.left2[13] = -20;

	camera.forwardSector[14] = 0;
	camera.rightSector[14] = 13;
	camera.backwardSector[14] = 15;
	camera.leftSector[14] = 21;
	camera.forward2[14] = 60;
	camera.right2[14] = -20;
	camera.backward2[14] = 55;
	camera.left2[14] = -15;

	camera.forwardSector[15] = 14;
	camera.rightSector[15] = 0;
	camera.backwardSector[15] = 16;
	camera.leftSector[15] = 0;
	camera.forward2[15] = 55;
	camera.right2[15] = -20;
	camera.backward2[15] = 40;
	camera.left2[15] = -15;

	camera.forwardSector[16] = 15;
	camera.rightSector[16] = 0;
	camera.backwardSector[16] = 17;
	camera.leftSector[16] = 28;
	camera.forward2[16] = 40;
	camera.right2[16] = -20;
	camera.backward2[16] = 35;
	camera.left2[16] = -15;

	camera.forwardSector[17] = 16;
	camera.rightSector[17] = 0;
	camera.backwardSector[17] = 18;
	camera.leftSector[17] = 0;
	camera.forward2[17] = 35;
	camera.right2[17] = -20;
	camera.backward2[17] = 25;
	camera.left2[17] = -15;

	camera.forwardSector[18] = 17;
	camera.rightSector[18] = 20;
	camera.backwardSector[18] = 4;
	camera.leftSector[18] = 0;
	camera.forward2[18] = 25;
	camera.right2[18] = -20;
	camera.backward2[18] = 20;
	camera.left2[18] = -15;

	camera.forwardSector[20] = 0;
	camera.rightSector[20] = 10;
	camera.backwardSector[20] = 0;
	camera.leftSector[20] = 18;
	camera.forward2[20] = 25;
	camera.right2[20] = -25;
	camera.backward2[20] = 20;
	camera.left2[20] = -20;

	camera.forwardSector[21] = 0;
	camera.rightSector[21] = 14;
	camera.backwardSector[21] = 0;
	camera.leftSector[21] = 22;
	camera.forward2[21] = 60;
	camera.right2[21] = -15;
	camera.backward2[21] = 55;
	camera.left2[21] = 15;

	camera.forwardSector[22] = 0;
	camera.rightSector[22] = 21;
	camera.backwardSector[22] = 23;
	camera.leftSector[22] = 0;
	camera.forward2[22] = 60;
	camera.right2[22] = 15;
	camera.backward2[22] = 55;
	camera.left2[22] = 20;

	camera.forwardSector[23] = 22;
	camera.rightSector[23] = 0;
	camera.backwardSector[23] = 24;
	camera.leftSector[23] = 0;
	camera.forward2[23] = 55;
	camera.right2[23] = 15;
	camera.backward2[23] = 40;
	camera.left2[23] = 20;

	camera.forwardSector[24] = 23;
	camera.rightSector[24] = 28;
	camera.backwardSector[24] = 25;
	camera.leftSector[24] = 0;
	camera.forward2[24] = 40;
	camera.right2[24] = 15;
	camera.backward2[24] = 35;
	camera.left2[24] = 20;

	camera.forwardSector[25] = 24;
	camera.rightSector[25] = 0;
	camera.backwardSector[25] = 26;
	camera.leftSector[25] = 0;
	camera.forward2[25] = 35;
	camera.right2[25] = 15;
	camera.backward2[25] = 20;
	camera.left2[25] = 20;

	camera.forwardSector[26] = 25;
	camera.rightSector[26] = 27;
	camera.backwardSector[26] = 0;
	camera.leftSector[26] = 0;
	camera.forward2[26] = 20;
	camera.right2[26] = 15;
	camera.backward2[26] = 15;
	camera.left2[26] = 20;

	camera.forwardSector[27] = 0;
	camera.rightSector[27] = 2;
	camera.backwardSector[27] = 0;
	camera.leftSector[27] = 26;
	camera.forward2[27] = 20;
	camera.right2[27] = 5;
	camera.backward2[27] = 15;
	camera.left2[27] = 15;

	camera.forwardSector[28] = 0;
	camera.rightSector[28] = 16;
	camera.backwardSector[28] = 0;
	camera.leftSector[28] = 24;
	camera.forward2[28] = 40;
	camera.right2[28] = -15;
	camera.backward2[28] = 35;
	camera.left2[28] = 15;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Dark Night (Egyptian edition)");
	glutDisplayFunc(Display);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutFullScreen();
	setupCamera();
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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
	glutTimerFunc(1, fpsTimer, 0);
	// INITIALIZERS
	initMenuEntries();
	glewInit();
	shaderId = configureShader();
	sManager.init(); // init Device (sound)
	sManager.loadAllSoundFiles(); // LOAD all Music and Sound files
	sManager.playMainMenuMusic();
	camera = Camera(2.5, 0.25, 2.5, 2.5, 0.25, 7.5, 0, 1, 0);
	initializeSectors();
	glutMainLoop();
}

void drawMainMenu() {
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	/* HUD ELEMENTs DRAWING*/
	renderString("AYAM SODA", WINDOW_WIDTH / 3.8, WINDOW_HEIGHT / 1.3, 0.75, 0, 0, 150);
	drawMenuEntry("Play", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 2, 0.5, 0, 0, 1);
	drawMenuEntry("Settings", WINDOW_WIDTH / 2.35, WINDOW_HEIGHT / 2.5, 0.5, 0, 0, 2);
	drawMenuEntry("Help", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 3.2, 0.5, 0, 0, 3);
	drawMenuEntry("Exit", WINDOW_WIDTH / 2.2, WINDOW_HEIGHT / 4.5, 0.5, 0, 0, 4);
	/*END OF ELEMENTS */
	// Making sure we can render 3d again
	glDepthMask(GL_TRUE);  // disable writes to Z-Buffer
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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

void drawMenuEntry(char *text, int x, int y, float r, float g, float b, int count) {
	menuEntries[count - 1].x = x;
	menuEntries[count - 1].y = y;
	if (menuEntries[count - 1].hover == true)
		renderString(text, x, y, 1, g, b, DEFAULT_ENTRY_SIZE);
	else
		renderString(text, x, y, r, g, b, DEFAULT_ENTRY_SIZE);
}

void mouseMenu(int button, int state, int x, int y) { // handle the main menu clicks
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

void drawHUD() {
	glUseProgram(0); // disable shaders (kont hat4al mn el 7war da)
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* HUD ELEMENTs DRAWING*/
	drawAdrenalineBar();
	fillAdrenalineBar(adrenalineLevel); // filled at 150
	/*END OF ELEMENTS */
	// Making sure we can render 3d again
	glUseProgram(shaderId);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}

void drawAdrenalineBar() {
	float x = 5, y = 6;
	glColor3f(0.75f, 0.75f, 0.75f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x), WINDOW_HEIGHT / y);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x) + MAX_ADRENALINE, WINDOW_HEIGHT / y);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x) + MAX_ADRENALINE, WINDOW_HEIGHT / y - 20);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x), WINDOW_HEIGHT / y - 20);
	glEnd();
}


void fillAdrenalineBar(float value) {
	float x = 5, y = 6;
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x), WINDOW_HEIGHT / y);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x) + value, WINDOW_HEIGHT / y);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x) + value, WINDOW_HEIGHT / y - 20);
	glVertex2f(WINDOW_WIDTH / x - WINDOW_WIDTH / (2 * x), WINDOW_HEIGHT / y - 20);
	glEnd();
}

void handleAdrenaline(int value) {
		adrenalineLevel += adrenalineIncValue;
		glutTimerFunc(adrenalineIncTime, handleAdrenaline, 0);
}

void fpsTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, fpsTimer, 0);
}