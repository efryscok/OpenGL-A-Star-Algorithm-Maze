#pragma once

class CCamera {
public:
	int width;
	int height;

	float movementSpeed;
	float fieldOfView;
	float aspectRatio;
	float nearPlane;
	float farPlane;

	glm::vec3 position;
	glm::vec3 lookAt;
	glm::vec3 upAxis;
	glm::vec3 offset;

	glm::quat orientation;

private:
	glm::mat4 mProjection;
	glm::mat4 mView;

public:
	CCamera();

	const glm::mat4& getProjection();
	const glm::mat4& getView();
	void update();
};

