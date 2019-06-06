#include "Data.h"

void Data::writeToBinary(FILE* fp)
{
	fwrite(&length, sizeof(size_t), 1, fp);
	fwrite(data.get(), 1, length, fp);
}

std::unique_ptr<Data> Data::readFromBinary(FILE* fp)
{
	std::unique_ptr<Data> ret = std::make_unique<Data>();
	size_t size;
	fread(&size, sizeof(size), 1, fp);
	ret->length = size;
	ret->data = std::make_unique<char[]>(size);
	fread(&ret->data[0], 1, size, fp);
	return ret;
}

