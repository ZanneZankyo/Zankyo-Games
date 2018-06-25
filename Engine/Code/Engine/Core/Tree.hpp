#pragma once
#include <vector>

class TreeNode
{
public:
	TreeNode* m_parent;
	std::vector<TreeNode*> m_children;

	TreeNode(TreeNode* parent = nullptr, const std::vector<TreeNode*>& children = std::vector<TreeNode*>());
};

class Tree
{
public:
	TreeNode* m_root;

public:

	Tree(TreeNode* root);
	static void GetAllNodes(std::vector<TreeNode*>& nodes, TreeNode* currentNode);
};

inline TreeNode::TreeNode(TreeNode* parent /*= nullptr*/, const std::vector<TreeNode*>& children /*= std::vector<TreeNode*>()*/)
	:m_parent(parent)
	,m_children(children)
{}

inline Tree::Tree(TreeNode* root)
	:m_root(root)
{}