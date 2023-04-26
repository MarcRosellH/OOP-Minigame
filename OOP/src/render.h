#pragma once
#ifndef __RENDERMODULE_H__
#define __RENDERMODULE_H__

#include <string>

#include <glad/glad.h>
#include <glad/khrplatform.h>

#include <glm/glm.hpp>

#include "module.h"
#include "input.h"

struct GLFWwindow;

struct Buffer
{
	GLuint			handle;
	GLenum			type;
	unsigned int    size;
	unsigned int    head;
	void*			data;
};

struct VertexBufferAttribute
{
	unsigned char	location;
	unsigned char	component_count;
	unsigned char	offset;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute>	attributes;
	unsigned char						stride;
};

struct VertexShaderAttribute
{
	unsigned char location;
	unsigned char component_count;
};

struct VertexShaderLayout
{
	std::vector<VertexShaderAttribute> attributes;
};

struct ShaderProgram
{
	GLuint					handle;
	std::string				file_path;
	std::string				name;
	VertexShaderLayout		vertex_input_layout;
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
	unsigned int load_shader_program(const char* _file_path, const char* _program_name);

public:
	// Public data

private:
	// Private data
	GLFWwindow* window;
	glm::ivec2	display_size;

	// Shader programs data
	std::vector<ShaderProgram> shader_programs;
	unsigned int deferred_geometry_pass_program_index;
	unsigned int deferred_lighting_pass_program_index;

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

	// Deferred rendering uniforms
	GLint deferred_geometry_program_uTexture;

	GLint deferred_lighting_program_uGPosition;
	GLint deferred_lighting_program_uGNormals;
	GLint deferred_lighting_program_uGDiffuse;

	// ---------------------------------------------
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

// Buffer Management

Buffer create_buffer(unsigned int _size, GLenum _type, GLenum _usage);

void push_aligned_data(Buffer& _buffer, const void* _data, unsigned int _size, unsigned int _alignment);

void bind_buffer(const Buffer& _buffer);
void align_head(Buffer& _buffer, unsigned int _alignment);

void map_buffer(Buffer& _buffer, GLenum _access);
void unmap_buffer(Buffer& _buffer);

bool is_power_of_2(unsigned int _value);

unsigned int align(unsigned int _value, unsigned int _alignment);

#define CreateConstantBuffer(_size) create_buffer(_size, GL_UNIFORM_BUFFER, GL_STREAM_DRAW);
#define CreateStaticVertexBuffer(_size) create_buffer(_size, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
#define CreateStaticIndexBuffer(_size) create_buffer(_size, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

#define PushData(_buffer, _data, _size) push_aligned_data(_buffer, _data, _size, 1);
#define PushUInt(_buffer, _value) {unsigned int v = _value; push_aligned_data(_buffer, &v, sizeof(v), 4);}
#define PushFloat(_buffer, _value) {float v = _value; push_aligned_data(_buffer, &v, sizeof(v), 4);}
#define PushVec3(_buffer, _value) push_aligned_data(_buffer, value_ptr(_value), sizeof(_value), sizeof(glm::vec4))
#define PushVec4(_buffer, _value) push_aligned_data(_buffer, value_ptr(_value), sizeof(_value), sizeof(glm::vec4))
#define PushMat3(_buffer, _value) push_aligned_data(_buffer, value_ptr(_value), sizeof(_value), sizeof(glm::vec4))
#define PushMat4(_buffer, _value) push_aligned_data(_buffer, value_ptr(_value), sizeof(_value), sizeof(glm::vec4))

// Shader loader

GLuint create_shader_program_from_source(std::string& _program_src, const char* _shader_name);

unsigned char get_attric_component_count(const GLenum& _type);

#endif // !__RENDERMODULE_H__