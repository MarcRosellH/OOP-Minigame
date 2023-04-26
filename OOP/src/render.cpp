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

RenderModule::RenderModule(App* _app) : Module(_app), window(nullptr), display_size(glm::ivec2(0,0)), deferred_geometry_pass_program_index(NULL), deferred_geometry_program_uTexture(NULL),
                                        deferred_lighting_pass_program_index(NULL), deferred_lighting_program_uGDiffuse(NULL), deferred_lighting_program_uGNormals(NULL),
                                        deferred_lighting_program_uGPosition(NULL), depth_attachment_handle(NULL), diffuse_attachment_handle(NULL), final_render_attachment_handle(NULL),
                                        framebuffer_final(NULL), framebuffer_geometry(NULL), normals_attachment_handle(NULL), position_attachment_handle(NULL)
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

    // [Deferred Render] Geometry Pass Program
    deferred_geometry_pass_program_index = load_shader_program("shaders.glsl", "DEFERRED_GEOMETRY_PASS");

    ShaderProgram& deferred_geo_pass_program = shader_programs[deferred_geometry_pass_program_index];
    GLint deferred_geo_attribute_count;
    glGetProgramiv(deferred_geo_pass_program.handle, GL_ACTIVE_ATTRIBUTES, &deferred_geo_attribute_count);

    for (GLint i = 0; i < deferred_geo_attribute_count; ++i)
    {
        GLchar attr_name[32];
        GLsizei attr_len;
        GLint attr_size;
        GLenum attr_type;

        glGetActiveAttrib(deferred_geo_pass_program.handle, i,
            ARRAY_COUNT(attr_name),
            &attr_len,
            &attr_size,
            &attr_type,
            attr_name);

        GLint attr_location = glGetAttribLocation(deferred_geo_pass_program.handle, attr_name);

        deferred_geo_pass_program.vertex_input_layout.attributes.push_back({ (unsigned char)attr_location, get_attric_component_count(attr_type) });
    }

    deferred_geometry_program_uTexture = glGetUniformLocation(deferred_geo_pass_program.handle, "uTexture");

    // [Deferred Render] Lighting Pass Program
    deferred_lighting_pass_program_index = load_shader_program("shaders.glsl", "DEFERRED_LIGHTING_PASS");

    ShaderProgram& deferred_lighting_pass_program = shader_programs[deferred_lighting_pass_program_index];
    GLint deferred_light_attribute_count;
    glGetProgramiv(deferred_lighting_pass_program.handle, GL_ACTIVE_ATTRIBUTES, &deferred_light_attribute_count);

    for (GLint i = 0; i < deferred_light_attribute_count; ++i)
    {
        GLchar attr_name[32];
        GLsizei attr_len;
        GLint attr_size;
        GLenum attr_type;

        glGetActiveAttrib(deferred_lighting_pass_program.handle, i,
            ARRAY_COUNT(attr_name),
            &attr_len,
            &attr_size,
            &attr_type,
            attr_name);

        GLint attr_location = glGetAttribLocation(deferred_lighting_pass_program.handle, attr_name);

        deferred_lighting_pass_program.vertex_input_layout.attributes.push_back({ (unsigned char)attr_location, get_attric_component_count(attr_type) });
    }

    deferred_lighting_program_uGPosition = glGetUniformLocation(deferred_lighting_pass_program.handle, "uGPosition");
    deferred_lighting_program_uGNormals = glGetUniformLocation(deferred_lighting_pass_program.handle, "uGNormals");
    deferred_lighting_program_uGDiffuse = glGetUniformLocation(deferred_lighting_pass_program.handle, "uGDiffuse");

    // [Framebuffers]

    // [Framebuffer] GBuffer
    // [Texture] Positions
    glGenTextures(1, &position_attachment_handle);
    glBindTexture(GL_TEXTURE_2D, position_attachment_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_size.x, display_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // [Texture] Normals
    glGenTextures(1, &normals_attachment_handle);
    glBindTexture(GL_TEXTURE_2D, normals_attachment_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_size.x, display_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // [Texture] Diffuse
    glGenTextures(1, &diffuse_attachment_handle);
    glBindTexture(GL_TEXTURE_2D, diffuse_attachment_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, display_size.x, display_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // [Texture] Depth
    glGenTextures(1, &depth_attachment_handle);
    glBindTexture(GL_TEXTURE_2D, depth_attachment_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, display_size.x, display_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &framebuffer_geometry);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_geometry);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, position_attachment_handle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normals_attachment_handle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, diffuse_attachment_handle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_attachment_handle, 0);

    GLenum drawBuffersGBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(ARRAY_COUNT(drawBuffersGBuffer), drawBuffersGBuffer);

    GLenum frameBufferStatusGBuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (frameBufferStatusGBuffer != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (frameBufferStatusGBuffer)
        {
        case GL_FRAMEBUFFER_UNDEFINED:                          LOG("Framebuffer status error: GL_FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:              LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:      LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED:                        LOG("Framebuffer status error: GL_FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:           LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;

        default: LOG("Unknown framebuffer status error"); break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // [Framebuffer] FBuffer
    glGenTextures(1, &final_render_attachment_handle);
    glBindTexture(GL_TEXTURE_2D, final_render_attachment_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, display_size.x, display_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &framebuffer_final);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_final);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, final_render_attachment_handle, 0);

    GLenum drawBuffersFBuffer[] = { GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(ARRAY_COUNT(drawBuffersFBuffer), drawBuffersFBuffer);

    GLenum frameBufferStatusFBuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (frameBufferStatusFBuffer != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (frameBufferStatusFBuffer)
        {
        case GL_FRAMEBUFFER_UNDEFINED:                          LOG("Framebuffer status error: GL_FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:              LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:      LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED:                        LOG("Framebuffer status error: GL_FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:             LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:           LOG("Framebuffer status error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;

        default: LOG("Unknown framebuffer status error"); break;
        }
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

bool is_power_of_2(unsigned int _value)
{
	return _value && !(_value & (_value - 1));
}

unsigned int align(unsigned int _value, unsigned int _alignment)
{
	return (_value + _alignment - 1) & ~(_alignment - 1);
}

Buffer create_buffer(unsigned int _size, GLenum _type, GLenum _usage)
{
	Buffer buffer = {};
	buffer.size = _size;
	buffer.type = _type;

	glGenBuffers(1, &buffer.handle);
	glBindBuffer(_type, buffer.handle);
	glBufferData(_type, buffer.size, NULL, _usage);
	glBindBuffer(_type, 0);

	return buffer;
}

void bind_buffer(const Buffer& _buffer)
{
	glBindBuffer(_buffer.type, _buffer.handle);
}

void map_buffer(Buffer& _buffer, GLenum _access)
{
	glBindBuffer(_buffer.type, _buffer.handle);
    _buffer.data = (unsigned char*)glMapBuffer(_buffer.type, _access);
    _buffer.head = 0;
}

void unmap_buffer(Buffer& _buffer)
{
	glUnmapBuffer(_buffer.type);
	glBindBuffer(_buffer.type, 0);
}

void align_head(Buffer& _buffer, unsigned int _alignment)
{
	ASSERT(is_power_of_2(_alignment), "The alignment must be a power of 2");
    _buffer.head = align(_buffer.head, _alignment);
}

void push_aligned_data(Buffer& _buffer, const void* _data, unsigned int _size, unsigned int _alignment)
{
	ASSERT(_buffer.data != NULL, "The buffer must be mapped first");
	align_head(_buffer, _alignment);
	memcpy((unsigned char*)_buffer.data + _buffer.head, _data, _size);
    _buffer.head += _size;
}

// Shader loader

GLuint create_shader_program_from_source(std::string& _program_src, const char* _shader_name)
{
    GLchar  info_log_buffer[1024] = {};
    GLsizei info_log_buffer_size = sizeof(info_log_buffer);
    GLsizei info_log_size;
    GLint   success;

    char version_string[] = "#version 430\n";
    char shader_name_define[128];
    sprintf(shader_name_define, "#define %s\n", _shader_name);
    char vertex_shader_define[] = "#define VERTEX\n";
    char fragment_shader_define[] = "#define FRAGMENT\n";

    const GLchar* vertex_shader_source[] = {
        version_string,
        shader_name_define,
        vertex_shader_define,
        _program_src.c_str()
    };
    const GLint vertex_shader_lengths[] = {
        (GLint)strlen(version_string),
        (GLint)strlen(shader_name_define),
        (GLint)strlen(vertex_shader_define),
        (GLint)_program_src.length()
    };
    const GLchar* fragment_shader_source[] = {
        version_string,
        shader_name_define,
        fragment_shader_define,
        _program_src.c_str()
    };
    const GLint fragment_shader_lengths[] = {
        (GLint)strlen(version_string),
        (GLint)strlen(shader_name_define),
        (GLint)strlen(fragment_shader_define),
        (GLint)_program_src.length()
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertex_shader_source), vertex_shader_source, vertex_shader_lengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, info_log_buffer_size, &info_log_size, info_log_buffer);
        LOG("glCompileShader() function failed with vertex shader %s\nReported message:\n%s\n", _shader_name, info_log_buffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragment_shader_source), fragment_shader_source, fragment_shader_lengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, info_log_buffer_size, &info_log_size, info_log_buffer);
        LOG("glCompileShader() function failed with fragment shader %s\nReported message:\n%s\n", _shader_name, info_log_buffer);
    }

    GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vshader);
    glAttachShader(program_handle, fshader);
    glLinkProgram(program_handle);
    glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program_handle, info_log_buffer_size, &info_log_size, info_log_buffer);
        LOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", _shader_name, info_log_buffer);
    }

    glUseProgram(0);

    glDetachShader(program_handle, vshader);
    glDetachShader(program_handle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return program_handle;
}

unsigned char get_attric_component_count(const GLenum& _type)
{
    switch (_type)
    {
    case GL_FLOAT: return 1; break;
    case GL_FLOAT_VEC2: return 2; break;
    case GL_FLOAT_VEC3: return 3; break;
    case GL_FLOAT_VEC4: return 4; break;
    case GL_FLOAT_MAT2: return 4; break;
    case GL_FLOAT_MAT3: return 9; break;
    case GL_FLOAT_MAT4: return 16; break;
    case GL_FLOAT_MAT2x3: return 6; break;
    case GL_FLOAT_MAT2x4: return 8; break;
    case GL_FLOAT_MAT3x2: return 6; break;
    case GL_FLOAT_MAT3x4: return 12; break;
    case GL_FLOAT_MAT4x2: return 8; break;
    case GL_FLOAT_MAT4x3: return 12; break;
    case GL_INT: return 1; break;
    case GL_INT_VEC2: return 2; break;
    case GL_INT_VEC3: return 3; break;
    case GL_INT_VEC4: return 4; break;
    case GL_UNSIGNED_INT: return 1; break;
    case GL_UNSIGNED_INT_VEC2: return 2; break;
    case GL_UNSIGNED_INT_VEC3: return 3; break;
    case GL_UNSIGNED_INT_VEC4: return 4; break;
    case GL_DOUBLE: return 1; break;
    case GL_DOUBLE_VEC2: return 2; break;
    case GL_DOUBLE_VEC3: return 3; break;
    case GL_DOUBLE_VEC4: return 4; break;
    case GL_DOUBLE_MAT2: return 4; break;
    case GL_DOUBLE_MAT3: return 9; break;
    case GL_DOUBLE_MAT4: return 16;
    case GL_DOUBLE_MAT2x3: return 6; break;
    case GL_DOUBLE_MAT2x4: return 8; break;
    case GL_DOUBLE_MAT3x2: return 6; break;
    case GL_DOUBLE_MAT3x4: return 12; break;
    case GL_DOUBLE_MAT4x2: return 8; break;
    case GL_DOUBLE_MAT4x3: return 12; break;
    default: return 0; break;
    }

    // leave this return 0 as a default return outisde the switch, just in case
    return 0;
}

unsigned int RenderModule::load_shader_program(const char* _file_path, const char* _program_name)
{
    std::string program_source = read_text_file(_file_path);

    ShaderProgram program = {};
    program.file_path = _file_path;
    program.name = _program_name;
    program.handle = create_shader_program_from_source(program_source, _program_name);
    shader_programs.push_back(program);
    return shader_programs.size() - 1;
}
