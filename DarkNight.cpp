#include <Camera.h>
#include <glut.h>

// FUNCTIONS DEFENITIONS
void drawDot(float x,float y,float z);
void keyboard(unsigned char key, int x, int y);
void mouse(int x, int y);
void fpsTimer(int value); // FPS (CONTROL THE RATE POST DISPLAY IS CALLED ( CURRENTLY 60)
void drawHUD();
						  
////////////////////////GAMEPLAY CONSTANTS////////////////////////////////////
float mouseSensitivity = 1.1;
float characterSpeed = 0.01;
///////////////////// DISPLAY CONSTANTS/////////////////////////////
const float WINDOW_WIDTH = 1366, WINDOW_HEIGHT = 768; //
const float FPS = 60.0;
float oldMouseX = WINDOW_WIDTH/2, oldMouseY = WINDOW_HEIGHT/2;

Camera camera = Camera(0.5, 0, 0.5, 0.4, 0, 0.4);

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
	gluPerspective(45.0, WINDOW_WIDTH / WINDOW_HEIGHT, 0.001, 50);

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
	drawHUD();
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

void main(int argc, char** argv) {
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
	glDepthMask(GL_TRUE);
	// REGISTER CALLBACKS
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(mouse);
	glutMainLoop();
}

void drawDot(float x ,float y, float z) {
	glPointSize(20);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(x, y, z);
	glEnd();
}

void drawHUD() {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* HUD ELEMENTs DRAWING*/
	/*  CROSSHAIR */
	drawDot(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0);
	glPopMatrix();
}