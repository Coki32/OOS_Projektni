#pragma once
#include <memory>
struct Data
{
	std::unique_ptr<char[]> data;
	size_t length;

	void writeToBinary(FILE* fp);
	static std::unique_ptr<Data> readFromBinary(FILE* fp);
};

