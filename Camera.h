using namespace std;
#include <vector>;

#ifndef CAMERA_HEADER
#define CAMERA_HEADER
#include "Vector3f.h"
#endif


class Camera {
public:
	Vector3f eye, center, direction, newCenter, newEye;
	int s;
	int rightSector[1000];
	int leftSector[1000];
	int forwardSector[1000];
	int backwardSector[1000];
	float right2[1000];
	float left2[1000];
	float forward2[1000];
	float backward2[1000];

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f);
	void updateSector();
	bool Camera::blocked();
	void moveForward(float d);
	void moveBackward(float d);
	void moveLeft(float d);
	void moveRight(float d);
	void rotateX(float a);
	void rotateY(float a);
	void look();
	
};

