#pragma once
#include <memory>
struct Data
{
	std::unique_ptr<char[]> data;
	size_t length;

	Data(const char* buffer, size_t bufferSize);

	void writeToBinary(FILE* fp);
	static std::unique_ptr<Data> readFromBinary(FILE* fp);

	std::shared_ptr<Data> removeNFromLeft(size_t n) const;
	std::shared_ptr<Data> append(const std::shared_ptr<Data>& other);
};

