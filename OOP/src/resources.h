#pragma once
#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <vector>
#include <string>

#include <glm\glm.hpp>

typedef unsigned int GLuint;

struct VertexBufferAttribute
{
    unsigned char location;
    unsigned char component_count;
    unsigned char offset;
};

struct VertexBufferLayout
{
    std::vector<VertexBufferAttribute>  attributes;
    unsigned char                       stride;
};

struct Model
{
    std::string                 name;
    unsigned int				mesh_index;
    std::vector<unsigned int>	material_index;
};

struct Vao
{
    GLuint  handle;
    GLuint  program_handle;
};

struct Submesh
{
    VertexBufferLayout              vertex_buffer_layout;
    std::vector<float>              vertices;
    std::vector<unsigned int>       indices;
    unsigned int                    vertex_offset;
    unsigned int                    index_offset;

    std::vector<Vao>                vaos;
};

struct Mesh
{
    std::vector<Submesh>    submeshes;
    GLuint                  vertex_buffer_handle;
    GLuint                  index_buffer_handle;
};

struct Material
{
    std::string          name;
    glm::vec3            albedo;
    glm::vec3            emissive;
    float                smoothness;
    unsigned int         albedo_texture_index;
    unsigned int         emissive_texture_index;
    unsigned int         specular_texture_index;
    unsigned int         normals_texture_index;
    unsigned int         bump_texture_index;
};

struct Image
{
    void*       pixels;
    glm::ivec2  size;
    int         nchannels;
    int         stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
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

#endif // !__RESOURCE_H__