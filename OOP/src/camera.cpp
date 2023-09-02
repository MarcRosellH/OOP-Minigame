#pragma once
#include "memleak.h"
#include "utils.h"

#include <glm/gtx/transform.hpp>

#include "component.h"
#include "camera.h"

ComponentCamera::ComponentCamera(Object* _parent) : Component(_parent), position(glm::vec3(1.F,1.F,1.F)), yaw(0), pitch(0), near_plane(0.1F), far_plane(1000.F)
{
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::start()
{

	update_vectors();
}

void ComponentCamera::update()
{
	update_vectors();
}

void ComponentCamera::clean_up()
{
}

glm::mat4 ComponentCamera::get_view_matrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 ComponentCamera::get_projection_matrix()
{
	return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
}

glm::vec3 ComponentCamera::get_position()
{
	return position;
}

void ComponentCamera::set_aspect_ratio(float dX, float dY)
{
	aspect_ratio = dX / dY;
}

void ComponentCamera::update_vectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}
