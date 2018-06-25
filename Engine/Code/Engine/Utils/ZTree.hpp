#pragma once
#include "Engine\Core\EngineBase.hpp"
#include <vector>

template <typename DataType>
class TreeNode
{
public:
	TreeNode* m_parent;
	std::vector<TreeNode*> m_children;
	DataType m_data;

public:

	TreeNode(const DataType& data, TreeNode* parent = nullptr) 
		: m_parent(parent), m_data(data), m_children() {}
	~TreeNode();
	void Push(const DataType& data);
	DataType& Data() { return m_data; }
	TreeNode* Parent() { return m_parent; }
	std::vector<TreeNode*>& Children() { return m_children; }
};

template <typename DataType>
TreeNode<DataType>::~TreeNode()
{
	for (auto& child : m_children)
		delete child;
}

template <typename DataType>
void TreeNode<DataType>::Push(const DataType& data)
{
	TreeNode<DataType>* newNode = new TreeNode<DataType>(data, this);
	m_children.push_back(newNode);
}

template <typename DataType>
class Tree
{
private:
	TreeNode<DataType>* m_head;

public:
	void SetHead(const DataType& data);
	TreeNode<DataType>* Head() { return m_head; };
	void Destroy();
	void OpenNew() { m_head = nullptr; }
	Tree();
	~Tree();
};

template <typename DataType>
Tree<DataType>::~Tree()
{
	Destroy();
}

template <typename DataType>
Tree<DataType>::Tree()
	: m_head(nullptr)
{}

template <typename DataType>
void Tree<DataType>::SetHead(const DataType& data)
{
	Destroy();
	m_head = new TreeNode<DataType>(data);
}

template <typename DataType>
void Tree<DataType>::Destroy()
{
	SAFE_DELETE(m_head);
}
