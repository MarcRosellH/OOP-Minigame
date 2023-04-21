#pragma once
#ifndef __RENDERMODULE_H__
#define __RENDERMODULE_H__

#include <glad/glad.h>
#include <glad/khrplatform.h>

#include <glm/glm.hpp>

#include "module.h"
#include "input.h"

struct GLFWwindow;

struct Buffer
{
	GLuint  handle;
	GLenum  type;
	unsigned int     size;
	unsigned int     head;
	void* data;
};

class RenderModule : public Module
{
public:
	// Public methods
	RenderModule(App* _app);
	virtual ~RenderModule();

	bool initialize() override;
	Update_State pre_update() override;
	Update_State post_update() override;
	bool clean_up() override;

	void set_display_size(const int _new_width, const int _new_height);

private:
	// Private methods

public:
	// Public data

private:
	// Private data
	GLFWwindow* window;
	glm::ivec2	display_size;
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

// Buffer Managemen

Buffer CreateBuffer(unsigned int size, GLenum type, GLenum usage);

void PushAlignedData(Buffer& buffer, const void* data, unsigned int size, unsigned int alignment);

void BindBuffer(const Buffer& buffer);
void AlignHead(Buffer& buffer, unsigned int alignment);

void MapBuffer(Buffer& buffer, GLenum access);
void UnmapBuffer(Buffer& buffer);

#define CreateConstantBuffer(size) CreateBuffer(size, GL_UNIFORM_BUFFER, GL_STREAM_DRAW);
#define CreateStaticVertexBuffer(size) CreateBuffer(size, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
#define CreateStaticIndexBuffer(size) CreateBuffer(size, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

#define PushData(buffer, data, size) PushAlignedData(buffer, data, size, 1);
#define PushUInt(buffer, value) {unsigned int v = value; PushAlignedData(buffer, &v, sizeof(v), 4);}
#define PushFloat(buffer, value) {float v = value; PushAlignedData(buffer, &v, sizeof(v), 4);}
#define PushVec3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushVec4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushMat3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))
#define PushMat4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(glm::vec4))

#endif // !__RENDERMODULE_H__