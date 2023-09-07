#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

class Object;
class ResourceManager;
class ComponentCamera;
class App;

class Scene
{
public:
    // Public methods
    Scene(App* _app, ResourceManager* _resource);
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

    unsigned long long total_created;
    unsigned long long total_destroyed;

    std::vector<Object*> to_delete_objects;

private:
    // Private data
    Object* root;
    Object* camera;

    ResourceManager* resource;

    App* app;
};

#endif // !__SCENE_H__