#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>

#include "component.h"

class Object;

class ComponentCamera : public Component
{
public:
	// Public methods
	ComponentCamera(Object* _parent);
	virtual ~ComponentCamera();

	void start() override;
	void update(float dt) override;
	void clean_up() override;

	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();

	glm::vec3 get_position();
	glm::vec3 position;

	void set_aspect_ratio(float dX, float dY);

private:
	// Private methods
	void update_vectors();

public:
	// Public data

private:
	// Private data
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