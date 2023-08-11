#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "glm/glm.hpp"

#include "component.h"

class Object;

class ComponentCamera : public Component
{
public:
	// Public methods
	ComponentCamera(Object* _parent);
	virtual ~ComponentCamera();

	void start() override;
	void update() override;
	void clean_up() override;

private:
	// Private methods

public:
	// Public data

private:
	// Private data
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;

	float yaw;
	float pitch;
	float fov;
	float near_plane;
	float far_plane;
	float aspect_ratio;
};

#endif // !__CAMERA_H__