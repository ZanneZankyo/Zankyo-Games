#include "MatrixStack.hpp"

MatrixStack::MatrixStack()
{
	PushDirect(Matrix4::IDENTITY);
}

void MatrixStack::Pop()
{
	if(m_matStacks.size() > 1)
		m_matStacks.pop();
}

void MatrixStack::Push(Matrix4 mat)
{
	mat.ConcatenateTransform(m_matStacks.top());
	m_matStacks.push(mat);
}

Matrix4 MatrixStack::Top()
{
	return m_matStacks.top();
}

void MatrixStack::PushDirect(const Matrix4& transform)
{
	m_matStacks.push(transform);
}
