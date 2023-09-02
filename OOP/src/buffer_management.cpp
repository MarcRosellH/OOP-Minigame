#pragma once

#include "utils.h"
#include "memleak.h"

#include "buffer_management.h"

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