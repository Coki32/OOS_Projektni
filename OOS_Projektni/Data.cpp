#include "Data.h"

Data::Data(const char* buffer, size_t bufferSize)
{
	length = bufferSize;
	data = std::make_unique<char[]>(length);
	std::copy(buffer, buffer + bufferSize, data.get());
}

void Data::writeToBinary(FILE* fp)
{
	fwrite(&length, sizeof(size_t), 1, fp);
	fwrite(data.get(), 1, length, fp);
}

std::unique_ptr<Data> Data::readFromBinary(FILE* fp)
{
	std::unique_ptr<Data> ret = std::make_unique<Data>(nullptr,0);
	size_t size;
	fread(&size, sizeof(size), 1, fp);
	ret->length = size;
	ret->data = std::make_unique<char[]>(size);
	fread(&ret->data[0], 1, size, fp);
	return ret;
}

std::shared_ptr<Data> Data::removeNFromLeft(size_t n) const
{
	if (n >= length)
		return std::make_shared<Data>(nullptr, 0);
	else
		return std::make_shared<Data>(data.get()+n, length-n);
}

std::shared_ptr<Data> Data::takeNFromLeft(size_t n) const
{
	if (n > this->length)
		throw std::exception("ne uzeti vise nego sto ima!");
	return std::make_shared<Data>(this->data.get(), n);
}

std::shared_ptr<Data> Data::append(const std::shared_ptr<Data>& other)
{
	size_t newLength = length + other->length;
	auto newData = std::make_shared<Data>(nullptr,0);
	newData->length = newLength;
	newData->data = std::make_unique<char[]>(newLength);
	std::copy(&data[0], &data[length], newData->data.get());
	std::copy(&other->data[0], &other->data[other->length], newData->data.get()+length);
	return newData;
}

