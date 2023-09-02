#pragma once

#include <glm/gtx/transform.hpp>

#include "memleak.h"
#include "utils.h"

#include "component.h"
#include "transform.h"

ComponentTransform::ComponentTransform(Object* _parent) : Component(_parent)
{
	position = rotation = scale = glm::vec3(0.0F, 0.0F, 0.0F);
}

ComponentTransform::ComponentTransform(Object* _parent, glm::vec3& _position, glm::vec3& _rotation, glm::vec3& _scale)
	: Component(_parent),
	position(_position), rotation(_rotation), scale(_scale)
{

}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::start()
{
}

void ComponentTransform::update()
{
}

void ComponentTransform::clean_up()
{
}

void ComponentTransform::set_position(glm::vec3 _new_position)
{
	position = _new_position;
}

void ComponentTransform::set_rotation(glm::vec3 _new_rotation)
{
	rotation = _new_rotation;
}

void ComponentTransform::set_scale(glm::vec3 _new_scale)
{
	scale = _new_scale;
}

const glm::vec3& ComponentTransform::get_position() const
{
	return position;
}

const glm::vec3& ComponentTransform::get_rotation() const
{
	return rotation;
}

const glm::vec3& ComponentTransform::get_scale() const
{
	return scale;
}

glm::mat4 ComponentTransform::matrix_position_rotation_scale()
{
	glm::mat4 ret = glm::translate(position);
	glm::vec3 radiant_rotation = glm::radians(rotation);
	ret = glm::rotate(ret, radiant_rotation.x, glm::vec3(1.F, 0.F, 0.F));
	ret = glm::rotate(ret, radiant_rotation.y, glm::vec3(0.F, 1.F, 0.F));
	ret = glm::rotate(ret, radiant_rotation.z, glm::vec3(0.F, 0.F, 1.F));
	return glm::scale(ret, scale);
}
