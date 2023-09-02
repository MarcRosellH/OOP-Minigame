#pragma once

//
// Assimp functions and loading data functions imported from advanced graphics programming project
//

#include "utils.h"
#include "memleak.h"

#include <glad/glad.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include "resource_manager.h"
#include "resources.h"

ResourceManager::ResourceManager(App* _app) : app(_app)
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::initial_load()
{

    load_model("Patrick/Patrick.obj");
    // LOAD SHADERS
    // 
    // [Deferred Render] Geometry Pass Program

    textured_mesh_index = load_shader_program("shaders.glsl", "TEXTURED_GEOMETRY");
    ShaderProgram& textured_mesh_program = shader_programs[textured_mesh_index];
    textured_program_uTexture = glGetUniformLocation(textured_mesh_program.handle, "uTexture");

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
}

unsigned int ResourceManager::get_model_index(const std::string& _model_name)
{
    for (unsigned int i = 0; i < models.size(); ++i)
    {
        if (models[i].name.compare(_model_name) == 0)
        {
            return i;
        }
    }
}

std::vector<RenderPackage>* ResourceManager::get_render_packs_vector()
{
    return &to_render;
}

void ResourceManager::add_render_pack(unsigned int _mesh_index, glm::mat4 _matrix)
{
    to_render.push_back({ _mesh_index, _matrix });
}

ShaderProgram& ResourceManager::get_shader_program(unsigned int _index)
{
    return shader_programs[_index];
}

unsigned int ResourceManager::get_geometry_shader_index()
{
    return deferred_geometry_pass_program_index;
}

unsigned int ResourceManager::get_lighting_shader_index()
{
    return deferred_lighting_pass_program_index;
}

unsigned int ResourceManager::get_textured_shader_index()
{
    return textured_mesh_index;
}

void ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Mesh* myMesh, unsigned int baseMeshMaterialIndex, std::vector<unsigned int>& submeshMaterialIndices)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            hasTexCoords = true;
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }

        if (mesh->mTangents != nullptr && mesh->mBitangents)
        {
            hasTangentSpace = true;
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);

            // For some reason ASSIMP gives me the bitangents flipped.
            // Maybe it's my fault, but when I generate my own geometry
            // in other files (see the generation of standard assets)
            // and all the bitangents have the orientation I expect,
            // everything works ok.
            // I think that (even if the documentation says the opposite)
            // it returns a left-handed tangent space matrix.
            // SOLUTION: I invert the components of the bitangent here.
            vertices.push_back(-mesh->mBitangents[i].x);
            vertices.push_back(-mesh->mBitangents[i].y);
            vertices.push_back(-mesh->mBitangents[i].z);
        }
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // store the proper (previously proceessed) material for this mesh
    submeshMaterialIndices.push_back(baseMeshMaterialIndex + mesh->mMaterialIndex);

    // create the vertex format
    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
    vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
    vertexBufferLayout.stride = 6 * sizeof(float);
    if (hasTexCoords)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 2 * sizeof(float);
    }
    if (hasTangentSpace)
    {
        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);

        vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, vertexBufferLayout.stride });
        vertexBufferLayout.stride += 3 * sizeof(float);
    }

    // add the submesh into the mesh
    Submesh submesh = {};
    submesh.vertex_buffer_layout = vertexBufferLayout;
    submesh.vertices.swap(vertices);
    submesh.indices.swap(indices);
    myMesh->submeshes.push_back(submesh);
}

void ResourceManager::ProcessAssimpMaterial(aiMaterial* material, Material& myMaterial, std::string directory)
{
    aiString name;
    aiColor3D diffuseColor;
    aiColor3D emissiveColor;
    aiColor3D specularColor;
    ai_real shininess;
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    material->Get(AI_MATKEY_SHININESS, shininess);

    myMaterial.name = name.C_Str();
    myMaterial.albedo = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    myMaterial.emissive = glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    myMaterial.smoothness = shininess / 256.0f;

    aiString aiFilename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiFilename);
        std::string filename = aiFilename.C_Str();
        std::string filepath = make_path(directory, filename);
        myMaterial.albedo_texture_index = load_texture_2D(filepath.c_str());
    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        material->GetTexture(aiTextureType_EMISSIVE, 0, &aiFilename);
        std::string filename = aiFilename.C_Str();
        std::string filepath = make_path(directory, filename);
        myMaterial.emissive_texture_index = load_texture_2D(filepath.c_str());
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        material->GetTexture(aiTextureType_SPECULAR, 0, &aiFilename);
        std::string filename = aiFilename.C_Str();
        std::string filepath = make_path(directory, filename);
        myMaterial.specular_texture_index = load_texture_2D(filepath.c_str());
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        material->GetTexture(aiTextureType_NORMALS, 0, &aiFilename);
        std::string filename = aiFilename.C_Str();
        std::string filepath = make_path(directory, filename);
        myMaterial.normals_texture_index = load_texture_2D(filepath.c_str());
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
        material->GetTexture(aiTextureType_HEIGHT, 0, &aiFilename);
        std::string filename =aiFilename.C_Str();
        std::string filepath = make_path(directory, filename);
        myMaterial.bump_texture_index = load_texture_2D(filepath.c_str());
    }

    //myMaterial.createNormalFromBump();
}

void ProcessAssimpNode(const aiScene* scene, aiNode* node, Mesh* myMesh, unsigned int baseMeshMaterialIndex, std::vector<unsigned int>& submeshMaterialIndices)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAssimpMesh(scene, mesh, myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessAssimpNode(scene, node->mChildren[i], myMesh, baseMeshMaterialIndex, submeshMaterialIndices);
    }
}

unsigned int ResourceManager::load_model(const char* _file_name)
{
    const aiScene* scene = aiImportFile(_file_name,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_OptimizeMeshes |
        aiProcess_SortByPType);

    if (!scene)
    {
        LOG("Error loading mesh %s: %s", _file_name, aiGetErrorString());
        return UINT32_MAX;
    }

    meshes.push_back(Mesh{});
    Mesh& mesh = meshes.back();
    unsigned int mesh_index = (unsigned int)meshes.size() - 1u;

    models.push_back(Model{});
    Model& model = models.back();
    model.mesh_index = mesh_index;
    model.name = get_filepath_name(_file_name);
    unsigned int modelIdx = (unsigned int)models.size() - 1u;

    std::string directory = get_directory_part(_file_name);

    // Create a list of materials
    unsigned int baseMeshMaterialIndex = (unsigned int)materials.size();
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        materials.push_back(Material{});
        Material& material = materials.back();
        ProcessAssimpMaterial(scene->mMaterials[i], material, directory);
    }

    ProcessAssimpNode(scene, scene->mRootNode, &mesh, baseMeshMaterialIndex, model.material_index);

    aiReleaseImport(scene);

    unsigned int vertexBufferSize = 0;
    unsigned int indexBufferSize = 0;

    for (unsigned int i = 0; i < mesh.submeshes.size(); ++i)
    {
        vertexBufferSize += mesh.submeshes[i].vertices.size() * sizeof(float);
        indexBufferSize += mesh.submeshes[i].indices.size() * sizeof(unsigned int);
    }

    glGenBuffers(1, &mesh.vertex_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.index_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

    unsigned int indicesOffset = 0;
    unsigned int verticesOffset = 0;

    for (unsigned int i = 0; i < mesh.submeshes.size(); ++i)
    {
        const void* verticesData = mesh.submeshes[i].vertices.data();
        const unsigned int   verticesSize = mesh.submeshes[i].vertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
        mesh.submeshes[i].vertex_offset = verticesOffset;
        verticesOffset += verticesSize;

        const void* indicesData = mesh.submeshes[i].indices.data();
        const unsigned int   indicesSize = mesh.submeshes[i].indices.size() * sizeof(unsigned int);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
        mesh.submeshes[i].index_offset = indicesOffset;
        indicesOffset += indicesSize;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    return modelIdx;
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

unsigned int ResourceManager::load_shader_program(const char* _file_path, const char* _program_name)
{
    std::string program_source = read_text_file(_file_path);

    ShaderProgram program = {};
    program.file_path = _file_path;
    program.name = _program_name;
    program.handle = create_shader_program_from_source(program_source, _program_name);
    shader_programs.push_back(program);
    return shader_programs.size() - 1;
}

unsigned int ResourceManager::load_texture_2D(const char* _file_path)
{
    for (unsigned int texIdx = 0; texIdx < textures.size(); ++texIdx)
        if (textures[texIdx].filepath == _file_path)
            return texIdx;

    Image image = load_image(_file_path);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = create_texture_2D_from_image(image);
        tex.filepath = _file_path;

        unsigned int texIdx = textures.size();
        textures.push_back(tex);

        free_image(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}

GLuint ResourceManager::create_texture_2D_from_image(Image _image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;
    GLenum dataType = GL_UNSIGNED_BYTE;

    switch (_image.nchannels)
    {
    case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
    case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
    default: LOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _image.size.x, _image.size.y, 0, dataFormat, dataType, _image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

Image ResourceManager::load_image(const char* _file_name)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(_file_name, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        LOG("Could not open file %s", _file_name);
    }
    return img;
}

void ResourceManager::free_image(Image image)
{
    stbi_image_free(image.pixels);
}
