#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#endif

class Camera
{
public:
	float eyeX, eyeY, eyeZ, centerX, centerY, centerZ;
	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f);
	void moveX(float d);
	void moveY(float d);
	void moveZ(float d);
	void rotateX(float a);
	void rotateY(float a);
	void look();
};

