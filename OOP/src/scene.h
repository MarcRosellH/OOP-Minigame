#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

class Object;
class ResourceManager;
class ComponentCamera;

#define MAX_OBJECTS 1024

class Scene
{
public:
    // Public methods
    Scene(ResourceManager* _resource);
    ~Scene();

    bool update();

    bool clean_up();

    bool delete_object(Object* _ref);

    ResourceManager* get_resource_manager();

public:
    // Private methods

public:
    // Public data
    ComponentCamera* camera_ref;

    unsigned int object_count;
    unsigned int total_count;
    unsigned int last_max_count;

    std::vector<Object*> to_delete_objects;

private:
    // Private data
    Object* root;
    Object* camera;

    ResourceManager* resource;
};

#endif // !__SCENE_H__