#include "Buffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

Buffer::Buffer(u64 size, const void* data, BufferUsage usage)
{
	Create();
	CreateStorage(size, data, usage);
}

void Buffer::Create()
{
	glCreateBuffers(1, &id);
}

void Buffer::Delete()
{
	glDeleteBuffers(1, &id);
	id = 0;
}

bool Buffer::IsValid() const
{
	return (id != 0) && (glIsBuffer(id) == GL_TRUE);
}

void Buffer::CreateStorage(u64 size, const void* data, BufferUsage usage) const
{
	glNamedBufferData(id, 
										size, 
										data, 
										static_cast<u32>(usage));
}

void Buffer::UpdateStorage(i32 offset, u32 size, const void* data) const
{
	glNamedBufferSubData(id, 
											 offset, 
											 size, 
											 data);
}

void* Buffer::MapStorage(BufferAccess access) const
{
	return glMapNamedBuffer(id, static_cast<u32>(access));
}

bool Buffer::UnmapStorage() const
{
	return glUnmapNamedBuffer(id);
}

void Buffer::BindBase(BufferTarget target, i32 bindingpoint) const
{
	glBindBufferBase(static_cast<u32>(target),
									 bindingpoint, 
									 id);
}

void Buffer::BindRange(BufferTarget target, i32 bindingpoint, i32 offset, u64 size) const
{
	glBindBufferRange(static_cast<u32>(target),
										bindingpoint,
										id,
										offset,
										size);
}

void Buffer::CopyFrom(u32 readBuffer, i32 readOffset, i32 writeOffset, u32 size) const
{
	u32 writeBuffer = this->id;
	glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);
}

i64 Buffer::GetBufferSize() const
{
	i64 size = 0;
	glGetNamedBufferParameteri64v(id, GL_BUFFER_SIZE, &size);
	return size;
}

i32 Buffer::GetBufferUsage() const
{
	i32 usage = GL_STATIC_DRAW;
	glGetNamedBufferParameteriv(id, GL_BUFFER_USAGE, &usage);
	return usage;
}
