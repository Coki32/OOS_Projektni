#pragma once
#include <cinttypes>
#include <vector>
#include "Blok.h"
#include "Data.h"

struct INode
{
	enum TYPE { FILE=0, FOLDER =1 };
	static const uint32_t NOT_SET = 0xFFFFFFFF;
	union {
		union {
			uint32_t permissiosns;
			struct {
				uint32_t unused : 3;//unused
				uint32_t user : 3;
				uint32_t group : 3;
				uint32_t others : 3;
				uint32_t usesExtents : 1;
				uint32_t type : 1;
				uint32_t nextNode : 16;
			};
		};//Total 14 zasad
		//Ako predjes 31 kriza
	};

	union {
		uint32_t blocks[12] = {
			NOT_SET, NOT_SET, NOT_SET,
			NOT_SET, NOT_SET, NOT_SET,
			NOT_SET, NOT_SET, NOT_SET,
			NOT_SET, NOT_SET, NOT_SET };
		uint32_t extentInfo[6][2];
	};
	uint32_t fileSize;

public:
	INode(uint32_t fileSize) : INode() {
		this->fileSize = fileSize;
	}
	INode() {
		permissiosns = 0;
		for (uint32_t i = 0; i < 12; i++)
			blocks[i] = NOT_SET;
		nextNode = (uint16_t)NOT_SET;
		fileSize = 0;
	}

	std::vector<int> getBlocks() const;
	std::vector<std::pair<int, int>> getExtents() const;
	inline size_t getActualSize() const {
		return sizeof(*this);
	}

	friend std::ostream& operator<<(std::ostream& os, const INode& node);

};

static_assert(sizeof(INode) == 56U, "NODE MORA BITI 56b");
