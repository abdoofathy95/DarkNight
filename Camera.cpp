#include <glut.h>
#include "Camera.h"
#include <math.h>
#include "Vector3f.h"
#define DEG2RAD(a) (a * 0.0174532925)

	Vector3f eye, center, up;
	Camera::Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void Camera::moveForward(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void Camera::moveBackward(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye - view * d;
		center = center - view * d;
	}

	void Camera::moveLeft(float d) {
		Vector3f left = up.cross(center - eye).unit();
		eye = eye + left * d;
		center = center + left * d;
	}

	void Camera::moveRight(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye - right * d;
		center = center - right * d;
	}

	void Camera::rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
	
		up = view.cross(right).unit();
		center = eye + view;
	}

	void Camera::rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up).unit();
		center = eye + view;
	}

	void Camera::look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
			);
	}