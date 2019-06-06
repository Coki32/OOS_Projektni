#pragma once

#define BLOCK_SIZE 128

struct Block
{
	char data[BLOCK_SIZE];
	inline size_t getActualSize() const {
		return sizeof(data);
	}
};

