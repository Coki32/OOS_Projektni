#pragma once
#include "Blok.h"
#include "Data.h"


class INode
{
public:
	union {
		int permissiosns;
		union {
			int dir : 3;
			int user : 3;
			int group : 3;
			int all : 3;
			int usesExtents : 1;
		};//Total 13 zasad
		//Ako predjes 31 kriza
	};

	union {
		int blocks[12];
		int extentInfo[6][2];
	};
	size_t nextNode;
public:
	INode() {
		permissiosns = 0;
		for (int i = 0; i < 12; i++)
			blocks[i] = -1;
		nextNode = 0;
	}
	void writeData(Block* dataSegment, Data *data);
	Data readData();

	inline size_t getActualSize() const {
		return sizeof(*this);
	}
};

static_assert(sizeof(INode) == 56U, "NODE MORA BITI 56b");
