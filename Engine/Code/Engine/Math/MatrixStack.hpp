#pragma once
#include <stack>
#include "Matrix4.hpp"

class MatrixStack
{
private:
	std::stack<Matrix4> m_matStacks;
public:
	MatrixStack();
	void Pop();
	void Push(Matrix4 mat);
	Matrix4 Top();
	void PushDirect(const Matrix4& transform);
};