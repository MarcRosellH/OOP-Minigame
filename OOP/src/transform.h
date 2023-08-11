#pragma once
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <glm/glm.hpp>

#include "component.h"

class Object;

class ComponentTransform : public Component
{
public:
	// Public methods
	ComponentTransform(Object* _parent);
	ComponentTransform(Object* _parent, glm::vec3& _position, glm::vec3& _rotation, glm::vec3& _scale);
	virtual ~ComponentTransform();

	void start() override;
	void update() override;
	void clean_up() override;

	void set_position(glm::vec3&);
	void set_rotation(glm::vec3&);
	void set_scale(glm::vec3&);

	const glm::vec3& get_position() const;
	const glm::vec3& get_rotation() const;
	const glm::vec3& get_scale() const;


private:
	// Private methods

public:
	// Public data

private:
	// Private data
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

#endif // !__TRANSFORM_H__