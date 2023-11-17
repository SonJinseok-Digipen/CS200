#include "GLVertexArray.h"
#include"glCheck.h"
#include <iostream>
GLVertexArray::GLVertexArray(GLPrimitive::Type the_primitive_pattern):primitive_pattern(the_primitive_pattern)
{
	glCheck(glCreateVertexArrays(1, &vertex_array_handle));
}

GLVertexArray::~GLVertexArray()
{
	glCheck(glDeleteVertexArrays(1, &vertex_array_handle));
}

GLVertexArray::GLVertexArray(GLVertexArray&& temp) noexcept
{
	vertex_array_handle = temp.vertex_array_handle;
	vertex_buffers = std::move(temp.vertex_buffers);
	index_buffer = std::move(temp.index_buffer);
	num_indices = temp.num_indices;
	indices_type = temp.indices_type;
	primitive_pattern = temp.primitive_pattern;
	num_vertices = temp.num_vertices;
	temp.vertex_array_handle = 0;

}

GLVertexArray& GLVertexArray::operator=(GLVertexArray&& temp) noexcept
{
	vertex_array_handle = temp.vertex_array_handle;
	vertex_buffers = std::move(temp.vertex_buffers);
	index_buffer = std::move(temp.index_buffer);
	num_indices = temp.num_indices;
	indices_type = temp.indices_type;
	primitive_pattern = temp.primitive_pattern;
	num_vertices = temp.num_vertices;
	temp.vertex_array_handle = 0;
	return *this;
}

void GLVertexArray::Use(bool bind) const
{
	if (bind == true)
	{
		glCheck(glBindVertexArray(vertex_array_handle));
	}
	else
	{
		glCheck(glBindVertexArray(0));
	}
}

void GLVertexArray::AddVertexBuffer(GLVertexBuffer&& vertex_buffer, std::initializer_list<GLAttributeLayout> buffer_layout)
{
	for (auto const& layout : buffer_layout)
	{
		glCheck(glEnableVertexArrayAttrib(vertex_array_handle,layout.vertex_layout_location));
		glCheck(glVertexArrayVertexBuffer(vertex_array_handle, layout.vertex_layout_location, vertex_buffer.GetHandle(),layout.offset,layout.stride));
		glCheck(glVertexArrayAttribFormat(vertex_array_handle, layout.vertex_layout_location, layout.component_dimension, layout.component_type, GL_FALSE, 0));
		glCheck(glVertexArrayAttribBinding(vertex_array_handle, layout.vertex_layout_location, layout.vertex_layout_location));

	}

}

void GLVertexArray::SetIndexBuffer(GLIndexBuffer&& the_indices)
{

	num_indices = the_indices.GetCount();
	num_vertices = the_indices.GetCount();
	indices_type = the_indices.GetElementType();

	glVertexArrayElementBuffer(vertex_array_handle, the_indices.GetHandle());

}

void GLDrawIndexed([[maybe_unused]] const GLVertexArray& vertex_array) noexcept
{

	glCheck(glDrawElements(vertex_array.GetPrimitivePattern(), vertex_array.GetIndicesCount(), vertex_array.GetIndicesType(), NULL));

}
void GLDrawVertices([[maybe_unused]] const GLVertexArray& vertex_array) noexcept
{


	glCheck(glDrawArrays(vertex_array.GetPrimitivePattern(), 0, vertex_array.GetVertexCount()));



}