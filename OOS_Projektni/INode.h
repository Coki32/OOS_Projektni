#pragma once
#include <cinttypes>
#include <vector>
#include "Blok.h"
#include "Data.h"

class INode
{
public:
	enum TYPE { FILE=0, FOLDER =1 };
	static const uint32_t NOT_SET = 0xFFFFFFFF;
	union {
		uint32_t permissiosns;
		union {
			uint32_t dir : 3;
			uint32_t user : 3;
			uint32_t group : 3;
			uint32_t all : 3;
			uint32_t usesExtents : 1;
			uint32_t type : 1;
		};//Total 13 zasad
		//Ako predjes 31 kriza
	};

	union {
		uint32_t blocks[12];
		uint32_t extentInfo[6][2];
	};
	uint16_t nextNode;
	uint16_t fileSize;
public:
	INode(uint16_t fileSize) : INode() {
		this->fileSize = fileSize;
	}
	INode() {
		permissiosns = 0;
		for (uint32_t i = 0; i < 12; i++)
			blocks[i] = NOT_SET;
		nextNode = (uint16_t)NOT_SET;
		fileSize = 0;
	}

	std::vector<int> getBlocks();

	inline size_t getActualSize() const {
		return sizeof(*this);
	}
};

static_assert(sizeof(INode) == 56U, "NODE MORA BITI 56b");
