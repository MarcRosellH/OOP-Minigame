#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

class Object;
class ResourceManager;
class ComponentCamera;

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
    void create_camera_object();

public:
    // Public data
    ComponentCamera* camera_ref;

private:
    // Private data
    Object* root;
    Object* camera;

    ResourceManager* resource;
};

#endif // !__SCENE_H__