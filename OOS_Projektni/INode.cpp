#include "INode.h"

std::vector<int> INode::getBlocks()
{
	std::vector<int> blocks;
	if (usesExtents)
		for (int ext = 0; ext < 6; ext++)
			for (int i = extentInfo[ext][0]; i <= extentInfo[ext][1]; i++)
				if (i != INode::NOT_SET)
					blocks.push_back(i);
				else
					break;
	else
		for (int i = 0; i < 12; i++)
			if (this->blocks[i] != INode::NOT_SET)
				blocks.push_back(blocks[i]);
			else
				break;
	return blocks;
}
