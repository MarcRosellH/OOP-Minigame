#pragma once

#include <Windows.h>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include <glfw3.h>

#include "app.h"
#include "render.h"
#include "memleak.h"

#include "resource_manager.h"
#include "resources.h"
#include "scene.h"
#include "camera.h"


#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "OOP Minigame"

#include "utils.h"

RenderModule::RenderModule(App* _app, ResourceManager* _resource, Scene* _scene_ref) : Module(_app), resource(_resource), scene_ref(_scene_ref), window(nullptr), display_size(glm::ivec2(0, 0)), depth_attachment_handle(NULL), diffuse_attachment_handle(NULL), final_render_attachment_handle(NULL),
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

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_buffer_size);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_block_alignment);

    uniform_buffer = CreateConstantBuffer(max_uniform_buffer_size);

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
	glClearColor(0.F, 0.F, 0.F, 1.F);
	return UPDATE_CONTINUE;
}

Update_State RenderModule::post_update()
{
    std::vector<RenderPackage>* packages = resource->get_render_packs_vector();

    scene_ref->camera_ref->set_aspect_ratio((float)display_size.x, (float)display_size.y);

    glm::mat4 projection_matrix = scene_ref->camera_ref->get_projection_matrix();
    glm::mat4 view_matrix = scene_ref->camera_ref->get_view_matrix();

    bind_buffer(uniform_buffer);
    uniform_buffer.data = (unsigned char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    uniform_buffer.head = 0;

    global_params_offset = uniform_buffer.head;

    PushVec3(uniform_buffer, scene_ref->camera_ref->position);
    PushUInt(uniform_buffer, lights.size());

    for (unsigned int i = 0; i < lights.size(); ++i)
    {
        align_head(uniform_buffer, sizeof(glm::vec4));

        Light& light = lights[i];
        /*PushUInt(uniform_buffer, light.type);
        PushVec3(uniform_buffer, light.color);
        PushVec3(uniform_buffer, light.direction);
        PushFloat(uniform_buffer, light.intensity);
        PushVec3(uniform_buffer, light.position);
        PushFloat(uniform_buffer, light.radius);*/
    }

    global_params_size = uniform_buffer.head - global_params_offset;

    for (unsigned int i = 0; i < (*packages).size(); ++i)
    {
        align_head(uniform_buffer, uniform_block_alignment);

        RenderPackage& ref = (*packages)[i];

        glm::mat4 world_view_projection_matrix = projection_matrix * view_matrix * ref.position_rotation_scale_matrix;

        ref.local_params_offset = uniform_buffer.head;
        
        /*PushMat4(uniform_buffer, ref.position_rotation_scale_matrix);
        PushMat4(uniform_buffer, world_view_projection_matrix);*/

        ref.local_params_size = uniform_buffer.head - ref.local_params_offset;
    }

    // Unmap buffer
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    /* First pass (geometry) */

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_geometry);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum drawBuffersGBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(ARRAY_COUNT(drawBuffersGBuffer), drawBuffersGBuffer);

    glViewport(0, 0, display_size.x, display_size.y);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_TRUE);

    ShaderProgram& deferredGeometryPassProgram = resource->get_shader_program(resource->get_geometry_shader_index());
    glUseProgram(deferredGeometryPassProgram.handle);


    for (const RenderPackage& entity : *packages)
    {
        Model& model = resource->models[entity.model_index];
        Mesh& mesh = resource->meshes[model.mesh_index];

        glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(1), uniform_buffer.handle, entity.local_params_offset, entity.local_params_size);

        for (unsigned int i = 0; i < mesh.submeshes.size(); ++i)
        {
            GLuint vao = find_vao(mesh, i, deferredGeometryPassProgram);
            //glBindVertexArray(vao);

            unsigned int submesh_material_index = model.material_index[i];
            Material& submesh_material = resource->materials[submesh_material_index];

            /*glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resource->textures[submesh_material.albedo_texture_index].handle);
            glUniform1i(resource->deferred_geometry_program_uTexture, 0);

            Submesh& submesh = mesh.submeshes[i];
            glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(unsigned long long)submesh.index_offset);

            glBindVertexArray(0);*/
        }
    }

    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Second pass (lighting) */
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Final pass");

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_final);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    GLenum drawBuffersFBuffer[] = { GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(ARRAY_COUNT(drawBuffersFBuffer), drawBuffersFBuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    //glDepthMask(GL_FALSE);

    ShaderProgram& deferredLightingPassProgram = resource->get_shader_program(resource->get_lighting_shader_index());
    glUseProgram(deferredLightingPassProgram.handle);

    glUniform1i(resource->deferred_lighting_program_uGPosition, 1);
    glUniform1i(resource->deferred_lighting_program_uGNormals, 2);
    glUniform1i(resource->deferred_lighting_program_uGDiffuse, 3);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, position_attachment_handle);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normals_attachment_handle);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, diffuse_attachment_handle);

    glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(0), uniform_buffer.handle, global_params_offset, global_params_size);


    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_geometry);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_final);

    glBlitFramebuffer(0, 0, display_size.x, display_size.y, 0, 0, display_size.x, display_size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render_quad();
    glPopDebugGroup();
    (*packages).clear();

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

GLuint RenderModule::find_vao(Mesh& _mesh, unsigned int _submesh_index, const ShaderProgram& _program)
{
    Submesh& submesh = _mesh.submeshes[_submesh_index];

    for (unsigned int i = 0; i < (unsigned int)submesh.vaos.size(); ++i)
    {
        if (submesh.vaos[i].program_handle == _program.handle)
            return submesh.vaos[i].handle;
    }

    GLuint vao_handle = 0;

    glGenVertexArrays(1, &vao_handle);
    glBindVertexArray(vao_handle);

    glBindBuffer(GL_ARRAY_BUFFER, _mesh.vertex_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh.index_buffer_handle);

    for (unsigned int i = 0; i < _program.vertex_input_layout.attributes.size(); ++i)
    {
        bool attributeWasLinked = false;

        for (unsigned int j = 0; j < submesh.vertex_buffer_layout.attributes.size(); ++j)
        {
            if (_program.vertex_input_layout.attributes[i].location == submesh.vertex_buffer_layout.attributes[j].location)
            {
                const unsigned int index = submesh.vertex_buffer_layout.attributes[j].location;
                const unsigned int ncomp = submesh.vertex_buffer_layout.attributes[j].component_count;
                const unsigned int offset = submesh.vertex_buffer_layout.attributes[j].offset + submesh.vertex_offset;
                const unsigned int stride = submesh.vertex_buffer_layout.stride;

                glVertexAttribPointer(index, ncomp, GL_FLOAT, GL_FALSE, stride, (void*)(unsigned long long)offset);
                glEnableVertexAttribArray(index);

                attributeWasLinked = true;
                break;
            }
        }
    }

    glBindVertexArray(0);

    Vao vao = { vao_handle, _program.handle };
    submesh.vaos.push_back(vao);

    return vao_handle;
}

void RenderModule::render_quad()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, display_size.x, display_size.y);

    ShaderProgram& quadProgram = resource->get_shader_program(resource->get_textured_shader_index());

    glUseProgram(quadProgram.handle);
    glBindVertexArray(quad.vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform1i(resource->textured_program_uTexture, 0);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, final_render_attachment_handle);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void RenderModule::generate_quad()
{
    glGenBuffers(1, &quad.embedded_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, quad.embedded_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad.vertices), quad.vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &quad.embedded_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.embedded_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad.indices), quad.indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &quad.vao);
    glBindVertexArray(quad.vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad.embedded_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.embedded_elements);
    glBindVertexArray(0);
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
