#pragma once
#include <memory>
#include "Data.h"
#define BLOCK_SIZE 128

struct Block
{
	char data[BLOCK_SIZE];
	inline size_t getActualSize() const {
		return sizeof(data);
	}
	inline std::shared_ptr<Data> getData() {
		return std::make_shared<Data>(data, BLOCK_SIZE);
	}
};

static_assert(sizeof(Block) == BLOCK_SIZE, "BLOK MORA BITI 128B!");
