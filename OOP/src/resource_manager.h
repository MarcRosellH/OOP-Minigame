#pragma once
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include <vector>
#include <string>

#include <glm\glm.hpp>

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int GLint;

class App;

struct Texture;
struct ShaderProgram;
struct Material;
struct Mesh;
struct Model;
struct Image;

struct aiMaterial;

struct RenderPackage
{
	unsigned int	model_index;
	glm::mat4		position_rotation_scale_matrix;
	unsigned int	local_params_offset;
	unsigned int	local_params_size;
};

class ResourceManager
{
public:
	// Public methods
    ResourceManager(App* _app);
	~ResourceManager();

	void initial_load();

	unsigned int get_model_index(const std::string& _model_name);

	std::vector<RenderPackage>* get_render_packs_vector();

	void add_render_pack(unsigned int _mesh_index, glm::mat4 _matrix);

	ShaderProgram& get_shader_program(unsigned int _index);

	unsigned int get_geometry_shader_index();
	unsigned int get_lighting_shader_index();
	unsigned int get_textured_shader_index();

private:
	// Private methods
	unsigned int load_model(const char* _file_name);

	void process_assimp_material(aiMaterial* material, Material& myMaterial, std::string directory);

	unsigned int load_shader_program(const char* _file_path, const char* _program_name);

	unsigned int load_texture_2D(const char* _file_path);

	GLuint create_texture_2D_from_image(Image _image);

	Image load_image(const char* _file_name);

	void free_image(Image image);

public:
	// Public data

	// Deferred rendering uniforms
	GLint deferred_geometry_program_uTexture;

	GLint deferred_lighting_program_uGPosition;
	GLint deferred_lighting_program_uGNormals;
	GLint deferred_lighting_program_uGDiffuse;
    
	std::vector<Texture>		textures;
    std::vector<ShaderProgram>  shader_programs;
    std::vector<Material>		materials;
    std::vector<Mesh>			meshes;
    std::vector<Model>			models;

	GLuint textured_program_uTexture;

private:
	// Private data
	App* app;


	// Shader programs data
	unsigned int deferred_geometry_pass_program_index;
	unsigned int deferred_lighting_pass_program_index;
	unsigned int textured_mesh_index;


	std::vector<RenderPackage> to_render;
};



// Shader loader

GLuint create_shader_program_from_source(std::string& _program_src, const char* _shader_name);

unsigned char get_attric_component_count(const GLenum& _type);

#endif // !__RESOURCE_MANAGER_H__