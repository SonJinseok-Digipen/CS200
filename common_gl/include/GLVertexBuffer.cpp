#include "GLVertexBuffer.h"
#include"glCheck.h"
GLVertexBuffer::GLVertexBuffer(GLsizei size_in_bytes):size(size_in_bytes)
{
	
	glCheck(glCreateBuffers(1, &buffer_handle));
	glCheck(glNamedBufferStorage(buffer_handle, size, nullptr, GL_DYNAMIC_STORAGE_BIT));
}

GLVertexBuffer::~GLVertexBuffer()
{
	glDeleteBuffers(1, &buffer_handle);
}

GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& temp) noexcept
{
	size = temp.size;
	buffer_handle = temp.buffer_handle;
	

	temp.buffer_handle = 0;
	temp.size = 0;

}



GLVertexBuffer& GLVertexBuffer::operator=(GLVertexBuffer&& temp) noexcept
{
	size = temp.size;
	buffer_handle = temp.buffer_handle;


	temp.buffer_handle = 0;
	temp.size = 0;
	return *this;
}

void GLVertexBuffer::Use(bool bind) const
{
	//We naver use this function in CS200 Assignments
	if (bind == true)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
}

void GLVertexBuffer::send_buffer_data_to_gpu(const void* data, GLsizei size_bytes, GLsizei starting_offset) const
{
	

	glCheck(glNamedBufferSubData(buffer_handle, starting_offset, size_bytes, data));
	
}
