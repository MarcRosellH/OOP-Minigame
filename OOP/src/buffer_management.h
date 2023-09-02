#pragma once
#ifndef __BUFFER_MANAGEMENT_H__
#define __BUFFER_MANAGEMENT_H__

#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

typedef unsigned int GLenum;

struct Buffer
{
	GLuint			handle;
	GLenum			type;
	unsigned int    size;
	unsigned int    head;
	void* data;
};

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

#endif // !__BUFFER_MANAGEMENT_H__