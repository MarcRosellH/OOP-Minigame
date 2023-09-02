#pragma once
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>

class Scene;
class Component;
class ComponentTransform;

enum class COMPONENT_TYPE;

enum class OBJECT_TYPE
{
	NONE,
	CAMERA,
	ENEMY,
	BULLET,
	MAX
};

class Object
{
public:
	// Public methods
	Object(Scene* _scene, bool _active);
	~Object();

	Component* add_component(COMPONENT_TYPE);

	void start();
	void update();
	void clean_up();

	void set_active(bool);

	Object* add_child();
	void delete_child(Object* _ref);

	void set_parent(Object* _ref);
	Object* get_parent() const;

	OBJECT_TYPE get_type() const;
	void set_type(OBJECT_TYPE _type);

	Scene* get_scene();

private:
	// Private methods

public:
	// Public data
	ComponentTransform* transform;

private:
	// Private data
	OBJECT_TYPE type;

	std::vector<Component*> components;
	bool active;

	std::vector<Object*> child;

	Object* parent;
	Scene* scene;
};

#endif // !__OBJECT_H__