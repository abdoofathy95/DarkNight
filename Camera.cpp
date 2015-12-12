using namespace std;
#include <vector>
#include <glut.h>
#include "Camera.h"
#include <math.h>
#define DEG2RAD(a) (a * 0.0174532925)

	

Vector3f eye, center, up, direction, newCenter, newEye;

	int s;
	int rightSector[1000];
	int leftSector[1000];
	int forwardSector[1000];
	int backwardSector[1000];
	float right[1000];
	float left[1000];
	float forward2[1000];
	float backward[1000];

Vector3f add(Vector3f u, Vector3f v) {
	return Vector3f(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vector3f sub(Vector3f u, Vector3f v) {
	return Vector3f(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vector3f mul(Vector3f u, float n) {
	return Vector3f(u.x * n, u.y * n, u.z * n);
}

Camera::Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
	eye = Vector3f(eyeX, eyeY, eyeZ);
	center = Vector3f(centerX, centerY, centerZ);
	up = Vector3f(upX, upY, upZ);
	s = 1;
}

void updateSector() {
	if(leftSector[s] && newEye.x < left[s]) s = leftSector[s];
	if(rightSector[s] && newEye.x > right[s]) s = rightSector[s];
	if(backwardSector[s] && newEye.z < backward[s]) s = backwardSector[s];
	if(forwardSector[s] && newEye.z > forward2[s]) s = forwardSector[s];
}

bool Camera::blocked() {
	return false;
	if(!leftSector[s] && newEye.x <= left[s] + 0.05) return true;
	if(!rightSector[s] && newEye.x >= right[s] - 0.05) return true;
	if(!backwardSector[s] && newEye.z <= backward[s] + 0.05) return true;
	if(!forwardSector[s] && newEye.z >= forward2[s] - 0.05) return true;
	return false;
}

void Camera::moveForward(float d) {
	Vector3f view = (center - eye).unit();
	newEye = eye + view * d;
	newCenter = center + view * d;
	if(blocked()) return;
	eye = newEye;
	center = newCenter;
}

void Camera::moveBackward(float d) {
	Vector3f view = (center - eye).unit();
	newEye = eye - view * d;
	newCenter = center - view * d;
	if(blocked()) return;
	eye = newEye;
	center = newCenter;
}

void Camera::moveLeft(float d) {
	Vector3f left = up.cross(center - eye).unit();
	newEye = eye + left * d;
	newCenter = center + left * d;
	if(blocked()) return;
	eye = newEye;
	center = newCenter;
}

void Camera::moveRight(float d) {
	Vector3f right = up.cross(center - eye).unit();
	newEye = eye - right * d;
	newCenter = center - right * d;
	if(blocked()) return;
	eye = newEye;
	center = newCenter;
}

void Camera::rotateX(float a) {
	Vector3f view = (center - eye).unit();
	Vector3f right = up.cross(view).unit();
	view = add(mul(view, cos(DEG2RAD(a))), mul(up, sin(DEG2RAD(a)))).unit();
	up = view.cross(right).unit();
	center = eye + view;
	direction = view;
}

void Camera::rotateY(float a) {
	Vector3f view = (center - eye).unit();
	Vector3f right = up.cross(view).unit();
	//    view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
	view = add(mul(view, cos(DEG2RAD(a))), mul(right, sin(DEG2RAD(a)))).unit();
	right = view.cross(up).unit();
	center = eye + view;
	direction = view;
}

void Camera::look() {
	gluLookAt(
		eye.x, eye.y, eye.z,
		center.x, center.y, center.z,
		up.x, up.y, up.z
		);
}