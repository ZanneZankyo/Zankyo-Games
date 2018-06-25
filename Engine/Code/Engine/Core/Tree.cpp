#include "Tree.hpp"

void Tree::GetAllNodes(std::vector<TreeNode*>& nodes, TreeNode* currentNode)
{
	nodes.push_back(currentNode);
	for (auto child : currentNode->m_children)
		GetAllNodes(nodes, child);
}

