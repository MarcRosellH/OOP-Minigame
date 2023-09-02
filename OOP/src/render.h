#pragma once
#ifndef __RENDERMODULE_H__
#define __RENDERMODULE_H__

#include <string>

#include <glad/glad.h>
#include <glad/khrplatform.h>

#include <glm\glm.hpp>

#include "module.h"
#include "input.h"
#include "buffer_management.h"

#define BINDING(b) b

enum LIGHT_TYPE
{
	LIGHTTYPE_DIRECTIONAL,
	LIGHTTYPE_POINT
};

struct Light
{
	LIGHT_TYPE	type;
	glm::vec3	color;
	glm::vec3	position;
	glm::vec3	direction;
	float		radius;
	float		intensity;
};

struct VertexV3V2
{
	glm::vec3 pos;
	glm::vec2 uv;
};

struct Quad
{
	GLuint vao;

	GLuint embedded_vertices;
	GLuint embedded_elements;

	VertexV3V2 vertices[4] = { glm::vec3(-1.0f, -1.0f, 0.0f),	glm::vec2(0.0f, 0.0f),
							   glm::vec3(1.0f, -1.0f, 0.0f),	glm::vec2(1.0f, 0.0f),
							   glm::vec3(1.0f,  1.0f, 0.0f),	glm::vec2(1.0f, 1.0f),
							   glm::vec3(-1.0f,  1.0f, 0.0f),	glm::vec2(0.0f, 1.0f)
	};

	unsigned short indices[6] = { 0, 1, 2, 0, 2, 3 };
};

struct Mesh;
struct ShaderProgram;
struct GLFWwindow;
class Scene;

class ResourceManager;

class RenderModule : public Module
{
public:
	// Public methods
	RenderModule(App* _app, ResourceManager* _resource, Scene* _scene_ref);
	virtual ~RenderModule();

	bool initialize() override;
	Update_State pre_update() override;
	Update_State post_update() override;
	bool clean_up() override;

	void set_display_size(const int _new_width, const int _new_height);

private:
	// Private methods

	GLuint find_vao(Mesh& _mesh, unsigned int submesh_index, const ShaderProgram& _program);

	void render_quad();

	void generate_quad();

public:
	// Public data
	Scene* scene_ref;

private:
	// Private data
	GLFWwindow* window;
	glm::ivec2	display_size;

	// Deferred rendering ---------------------------

	// Deferred rendering framebuffers
	GLuint framebuffer_geometry;
	GLuint framebuffer_final;

	// Deferred rendering attachment handles
	GLuint position_attachment_handle;
	GLuint normals_attachment_handle;
	GLuint diffuse_attachment_handle;
	GLuint depth_attachment_handle;

	GLuint final_render_attachment_handle;

	// ---------------------------------------------

	GLint	max_uniform_buffer_size;
	GLint	uniform_block_alignment;
	Buffer	uniform_buffer;
	
	unsigned int global_params_offset;
	unsigned int global_params_size;

	Quad quad;

	std::vector<Light> lights;

	ResourceManager* resource;
};

// OpenGL callbacks

void on_gl_error(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar* _message, const void* _user_param);

void on_glfw_mouse_event(GLFWwindow* _window, int _button, int _event, int _modifiers);

void on_glfw_mouse_move_event(GLFWwindow* _window, double _xposition, double _yposition);

void on_glfw_scroll_event(GLFWwindow* _window, double _xoffset, double _yoffset);

void on_glfw_keyboard_event(GLFWwindow* _window, int _key, int _scancode, int _action, int _modifications);

void on_glfw_char_event(GLFWwindow* _window, unsigned int _character);

void on_glfw_resize_framebuffer(GLFWwindow* _window, int _width, int _height);

void on_glfw_close_window(GLFWwindow* _window);

#endif // !__RENDERMODULE_H__