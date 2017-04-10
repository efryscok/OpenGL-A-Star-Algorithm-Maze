#include "global.h"

CCamera::CCamera() {
	width = 1200;
	height = 800;
	movementSpeed = 1.f;
	fieldOfView = 1.f;
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	nearPlane = .01f;
	farPlane = 1000.f;
	
	upAxis.y = 1.f;
}

const glm::mat4& CCamera::getProjection() {
	return mProjection;
}

const glm::mat4& CCamera::getView() {
	return mView;
}

void CCamera::update() {
	mProjection = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
	mView = glm::lookAt(position, lookAt, upAxis);
}
