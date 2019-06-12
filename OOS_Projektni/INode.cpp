#include <string>
#include <iostream>
#include "INode.h"

std::vector<int> INode::getBlocks() const
{
	std::vector<int> blocks;
	if (usesExtents)
		for (int ext = 0; ext < 6; ext++)
			for (uint32_t i = extentInfo[ext][0]; i <= extentInfo[ext][1]; i++)
				if (i != INode::NOT_SET)
					blocks.push_back(i);
				else
					break;
	else
		for (uint32_t i = 0; i < 12; i++)
			if (this->blocks[i] != INode::NOT_SET)
				blocks.push_back(blocks[i]);
			else
				break;
	return blocks;
}

std::vector<std::pair<int, int>> INode::getExtents() const
{
	std::vector<std::pair<int, int>> extents;
	for (int i = 0; i < 6; i++)
		if (extentInfo[i][0] == NOT_SET)
			break;
		else
			extents.push_back(std::make_pair(extentInfo[i][0], extentInfo[i][1]));
	return extents;
}

static inline std::string permToStr(int perm) {
	std::string rez = "---";
	if (perm & 0x1)
		rez[2] = 'x';
	if (perm & 0x2)
		rez[1] = 'w';
	if (perm & 0x4)
		rez[0] = 'r';
	return rez;
}

static inline std::string dirToStr(int dir) {
	if (dir == INode::TYPE::FOLDER)
		return "d";
	else
		return "f";
}

std::ostream& operator<<(std::ostream& os, const INode& node)
{
	os <<"Access: "<< dirToStr(node.type) << permToStr(node.user) << permToStr(node.group) << permToStr(node.others) << std::endl;
	os << "Blocks: ";
	if (node.usesExtents) {
		for (int i = 0; i < 6; i++)
			if (node.extentInfo[i][0] == INode::NOT_SET)
				break;
			else
				os << "[ " << node.extentInfo[i][0] << " - " << node.extentInfo[i][1] << " ] ";
	}
	else
		for (int i = 0; i < 12; i++)
			if (node.blocks[i] == INode::NOT_SET)
				break;
			else
				os << "[" << node.blocks[i] << "] ";
	os << std::endl << "Next node: " << (node.nextNode == (uint16_t)INode::NOT_SET ? "none" : std::to_string(node.nextNode)) << std::endl;
	os << "File size: " << node.fileSize << " b" << std::endl;
	os << "Size on disk: " << node.getBlocks().size() * BLOCK_SIZE << std::endl;
	return os;
}
