#include "Camera.h"
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

// FUNCTIONS DEFENITIONS
void drawAxis();
void drawGrid(int x, int y);
void keyboard(unsigned char key, int x, int y);
void mouse(int x, int y);
void fpsTimer(int value); // FPS (CONTROL THE RATE POST DISPLAY IS CALLED ( CURRENTLY 60)

						  
////////////////////////GAMEPLAY CONSTANTS////////////////////////////////////
float mouseSensitivity = 1.1;
float characterSpeed = 0.01;
///////////////////// DISPLAY CONSTANTS/////////////////////////////
const float WINDOW_WIDTH = 1366, WINDOW_HEIGHT = 768; //
const float FPS = 60.0;
float oldMouseX = WINDOW_WIDTH/2, oldMouseY = WINDOW_HEIGHT/2;

Camera camera = Camera(0.5, 0, 0.5, 1, 1, 1);

void drawWall(double thickness) {
	glPushMatrix();
	glTranslated(0.5 * thickness, -0.001 , 0.5 * thickness);
	glScaled(thickness, 0.001, thickness);
	glutSolidCube(1);
	glPopMatrix();
}


void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, WINDOW_WIDTH / WINDOW_HEIGHT, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void Display() {
	setupCamera();
	setupLights();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.752941, 0.752941, 0.752941);
	drawWall(100);
	glFlush();
}

void Keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'w':
		camera.moveZ(characterSpeed);
		break;
	case 's':
		camera.moveZ(-characterSpeed);
		break;
	case 'a':
		camera.moveX(characterSpeed);
		break;
	case 'd':
		camera.moveX(-characterSpeed);
		break;
	}

	glutPostRedisplay();
}

void mouse(int x, int y) {
	//camera.rotateX((oldMouseY-y)*mouseSensitivity);
	camera.rotateY((oldMouseX-x)*mouseSensitivity);
	oldMouseX = x;
	//oldMouseY = y;
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Dark Night (Egyptian edition)");
	glutDisplayFunc(Display);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2); // Position mouse at middle of screen
	glEnable(GL_DEPTH_TEST); // ENABLE PERSPECTIVE CAMERA
	glEnable(GL_LIGHTING);	// ENABLE LIGHT 

	glEnable(GL_NORMALIZE); 	// ENABLE NORMALIZE OF VECTORS AFTER ANY OPERATION 
	glEnable(GL_COLOR_MATERIAL); // ENABLE COLOR MATERIAL
	glShadeModel(GL_SMOOTH); // ENABLE SHADING

	// REGISTER CALLBACKS
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(mouse);
	glutMainLoop();
}
