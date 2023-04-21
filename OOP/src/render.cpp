#pragma once

#include <Windows.h>
#include <stdio.h>

#include "app.h"
#include "render.h"
#include "memleak.h"

#include <glfw3.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "OOP Minigame"

#include "utils.h"

RenderModule::RenderModule(App* _app) : Module(_app), window(nullptr), display_size(glm::ivec2(0,0))
{
}

RenderModule::~RenderModule()
{
}

bool RenderModule::initialize()
{
	glfwSetErrorCallback(on_glfw_error);

	if (!glfwInit())
	{
		LOG("glfwInit() method failed\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	display_size = glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (!window)
	{
		LOG("glfwCreateWindow() method failed!\n");
		return false;
	}

	glfwSetWindowUserPointer(window, app);

	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, on_glfw_mouse_event);
	glfwSetCursorPosCallback(window, on_glfw_mouse_move_event);
	glfwSetScrollCallback(window, on_glfw_scroll_event);
	glfwSetKeyCallback(window, on_glfw_keyboard_event);
	glfwSetCharCallback(window, on_glfw_char_event);
	glfwSetFramebufferSizeCallback(window, on_glfw_resize_framebuffer);
	glfwSetWindowCloseCallback(window, on_glfw_close_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG("Failed to initialize OpenGL context\n");
		return false;
	}

	if (GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3))
	{
		glDebugMessageCallback(on_gl_error, app);
	}


	return true;
}

Update_State RenderModule::pre_update()
{
	glClearColor(0.F, 1.F, 0.F, 1.F);
	return UPDATE_CONTINUE;
}

Update_State RenderModule::post_update()
{
	glfwSwapBuffers(window);
	return UPDATE_CONTINUE;
}

bool RenderModule::clean_up()
{
	glfwDestroyWindow(window);

	glfwTerminate();
	return true;
}

void RenderModule::set_display_size(const int _new_width, const int _new_height)
{
	display_size = glm::ivec2(_new_width, _new_height);
}

// OpenGL callbacks

void on_gl_error(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar* _message, const void* _user_param)
{
    if (_severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    LOG("OpenGL debug message: %s", _message);

    switch (_source)
    {
    case GL_DEBUG_SOURCE_API:               LOG(" - source: GL_DEBUG_SOURCE_API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     LOG(" - source: GL_DEBUG_SOURCE_WINDOW_SYSTEM"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   LOG(" - source: GL_DEBUG_SOURCE_SHADER_COMPILER"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       LOG(" - source: GL_DEBUG_SOURCE_THIRD_PARTY"); break;
    case GL_DEBUG_SOURCE_APPLICATION:       LOG(" - source: GL_DEBUG_SOURCE_APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER:             LOG(" - source: GL_DEBUG_SOURCE_OTHER"); break;
    }

    switch (_type)
    {
    case GL_DEBUG_TYPE_ERROR:               LOG(" - type: GL_DEBUG_TYPE_ERROR"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: LOG(" - type: GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  LOG(" - type: GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"); break;
    case GL_DEBUG_TYPE_PORTABILITY:         LOG(" - type: GL_DEBUG_TYPE_PORTABILITY"); break;
    case GL_DEBUG_TYPE_PERFORMANCE:         LOG(" - type: GL_DEBUG_TYPE_PERFORMANCE"); break;
    case GL_DEBUG_TYPE_MARKER:              LOG(" - type: GL_DEBUG_TYPE_MARKER"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          LOG(" - type: GL_DEBUG_TYPE_PUSH_GROUP"); break;
    case GL_DEBUG_TYPE_POP_GROUP:           LOG(" - type: GL_DEBUG_TYPE_POP_GROUP"); break;
    case GL_DEBUG_TYPE_OTHER:               LOG(" - type: GL_DEBUG_TYPE_OTHER"); break;
    }

    switch (_severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:            LOG(" - severity: GL_DEBUG_SEVERITY_HIGH"); break;
    case GL_DEBUG_SEVERITY_MEDIUM:          LOG(" - severity: GL_DEBUG_SEVERITY_MEDIUM"); break;
    case GL_DEBUG_SEVERITY_LOW:             LOG(" - severity: GL_DEBUG_SEVERITY_LOW"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    LOG(" - severity: GL_DEBUG_SEVERITY_NOTIFICATION"); break;
    }
}

void on_glfw_mouse_event(GLFWwindow* _window, int _button, int _event, int _modifiers)
{
	App* app = (App*)glfwGetWindowUserPointer(_window);

	switch (_event)
	{
	case GLFW_PRESS:
		switch (_button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT: 
			(app->input->mouse_buttons[RIGHT] != BUTTON_PRESSED) ? app->input->mouse_buttons[RIGHT] = BUTTON_PRESS : app->input->mouse_buttons[RIGHT] == BUTTON_PRESSED; break;
		case GLFW_MOUSE_BUTTON_LEFT:
			(app->input->mouse_buttons[LEFT] != BUTTON_PRESSED) ? app->input->mouse_buttons[LEFT] = BUTTON_PRESS : app->input->mouse_buttons[LEFT] == BUTTON_PRESSED; break;
		}
		break;
	case GLFW_RELEASE:
		switch (_button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT: app->input->mouse_buttons[RIGHT] = BUTTON_RELEASE; break;
		case GLFW_MOUSE_BUTTON_LEFT:  app->input->mouse_buttons[LEFT] = BUTTON_RELEASE; break;
		}
		break;
	}
}

void on_glfw_mouse_move_event(GLFWwindow* _window, double _xposition, double _yposition)
{
	App* app = (App*)glfwGetWindowUserPointer(_window);

	app->input->mouse_delta.x = _xposition - app->input->mouse_position.x;
	app->input->mouse_delta.y = _xposition - app->input->mouse_position.y;
	app->input->mouse_position.x = _xposition;
	app->input->mouse_position.y = _yposition;
}

void on_glfw_scroll_event(GLFWwindow* _window, double _xoffset, double _yoffset)
{
	// Not managed...
}

void on_glfw_keyboard_event(GLFWwindow* _window, int _key, int _scancode, int _action, int _modifications)
{
	// Remap key to our enum values
	switch (_key)
	{
	case GLFW_KEY_SPACE:  _key = K_SPACE; break;
	case GLFW_KEY_0: _key = K_0; break; case GLFW_KEY_1: _key = K_1; break; case GLFW_KEY_2: _key = K_2; break;
	case GLFW_KEY_3: _key = K_3; break; case GLFW_KEY_4: _key = K_4; break; case GLFW_KEY_5: _key = K_5; break;
	case GLFW_KEY_6: _key = K_6; break; case GLFW_KEY_7: _key = K_7; break; case GLFW_KEY_8: _key = K_8; break;
	case GLFW_KEY_9: _key = K_9; break;
	case GLFW_KEY_A: _key = K_A; break; case GLFW_KEY_B: _key = K_B; break; case GLFW_KEY_C: _key = K_C; break;
	case GLFW_KEY_D: _key = K_D; break; case GLFW_KEY_E: _key = K_E; break; case GLFW_KEY_F: _key = K_F; break;
	case GLFW_KEY_G: _key = K_G; break; case GLFW_KEY_H: _key = K_H; break; case GLFW_KEY_I: _key = K_I; break;
	case GLFW_KEY_J: _key = K_J; break; case GLFW_KEY_K: _key = K_K; break; case GLFW_KEY_L: _key = K_L; break;
	case GLFW_KEY_M: _key = K_M; break; case GLFW_KEY_N: _key = K_N; break; case GLFW_KEY_O: _key = K_O; break;
	case GLFW_KEY_P: _key = K_P; break; case GLFW_KEY_Q: _key = K_Q; break; case GLFW_KEY_R: _key = K_R; break;
	case GLFW_KEY_S: _key = K_S; break; case GLFW_KEY_T: _key = K_T; break; case GLFW_KEY_U: _key = K_U; break;
	case GLFW_KEY_V: _key = K_V; break; case GLFW_KEY_W: _key = K_W; break; case GLFW_KEY_X: _key = K_X; break;
	case GLFW_KEY_Y: _key = K_Y; break; case GLFW_KEY_Z: _key = K_Z; break;
	case GLFW_KEY_ESCAPE: _key = K_ESCAPE; break;
	case GLFW_KEY_ENTER:  _key = K_ENTER; break;
	}

	App* app = (App*)glfwGetWindowUserPointer(_window);
	switch (_action)
	{
	case GLFW_PRESS:   (app->input->keys[_key] != BUTTON_PRESSED) ? app->input->keys[_key] = BUTTON_PRESS : app->input->keys[_key] == BUTTON_PRESSED; break;
	case GLFW_RELEASE: app->input->keys[_key] = BUTTON_RELEASE; break;
	}
}

void on_glfw_char_event(GLFWwindow* _window, unsigned int _character)
{
	// Not managed...
}

void on_glfw_resize_framebuffer(GLFWwindow* _window, int _width, int _height)
{
	App* app = (App*)glfwGetWindowUserPointer(_window);
	app->renderer->set_display_size(_width, _height);
}

void on_glfw_close_window(GLFWwindow* _window)
{
	App* app = (App*)glfwGetWindowUserPointer(_window);
	app->quit_app();
}

// Buffer Management

bool IsPowerOf2(unsigned int value)
{
	return value && !(value & (value - 1));
}

unsigned int Align(unsigned int value, unsigned int alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

Buffer CreateBuffer(unsigned int size, GLenum type, GLenum usage)
{
	Buffer buffer = {};
	buffer.size = size;
	buffer.type = type;

	glGenBuffers(1, &buffer.handle);
	glBindBuffer(type, buffer.handle);
	glBufferData(type, buffer.size, NULL, usage);
	glBindBuffer(type, 0);

	return buffer;
}

void BindBuffer(const Buffer& buffer)
{
	glBindBuffer(buffer.type, buffer.handle);
}

void MapBuffer(Buffer& buffer, GLenum access)
{
	glBindBuffer(buffer.type, buffer.handle);
	buffer.data = (unsigned char*)glMapBuffer(buffer.type, access);
	buffer.head = 0;
}

void UnmapBuffer(Buffer& buffer)
{
	glUnmapBuffer(buffer.type);
	glBindBuffer(buffer.type, 0);
}

void AlignHead(Buffer& buffer, unsigned int alignment)
{
	ASSERT(IsPowerOf2(alignment), "The alignment must be a power of 2");
	buffer.head = Align(buffer.head, alignment);
}

void PushAlignedData(Buffer& buffer, const void* data, unsigned int size, unsigned int alignment)
{
	ASSERT(buffer.data != NULL, "The buffer must be mapped first");
	AlignHead(buffer, alignment);
	memcpy((unsigned char*)buffer.data + buffer.head, data, size);
	buffer.head += size;
}