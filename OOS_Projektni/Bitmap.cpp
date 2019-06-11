#include <cmath>
#include <memory>
#include <exception>
#include <tuple>

#include "Bitmap.h"

#include "Blok.h"

#define SIZE_OF_ONE (sizeof(char))

Bitmap::Bitmap(int numberOfBits, size_t sizeOfOneBit)
{
	this->sizeOfOneBit = sizeOfOneBit;
	this->numberOfChars = (int)std::ceil(numberOfBits / sizeof(char));

	this->numberOfBits = numberOfBits;
	this->bits = std::make_unique<char[]>(numberOfChars);
	std::memset(this->bits.get(), 0, numberOfChars);

	currentBit = 0;
}

size_t Bitmap::findNextFreeField()
{
	for (int steps = 0; steps < this->numberOfBits; steps++) {
		int charIdx = currentBit / SIZE_OF_ONE;
		int bitIdx = currentBit % SIZE_OF_ONE;
		if (!((bits)[charIdx] & (1 << bitIdx))) {
			return currentBit;
		}
		currentBit = (currentBit + 1) % numberOfBits;
	}
	return -1;
}


std::vector<std::pair<int, int>> Bitmap::findExtentStart_v(size_t extentSizeInBytes)
{
	std::vector<std::pair<int, int>> ret;
	int numberOfBitsRequired = (int)std::ceil((double)extentSizeInBytes / sizeOfOneBit);
	if (numberOfBitsRequired > numberOfBits)
		return ret;//ne moze nikad imati vise nego sto ima...
	int curr = currentBit;
	do {
		if (getBit(curr) == 0) {//ako je slobodan ovaj, trazi dalje
			int start = curr;
			int end = start;
			while (getBit(end) == 0 && (end - start + 1) <= numberOfBitsRequired && end<this->numberOfBits)
				end++;
			if (end > start) {
				ret.push_back(std::make_pair(start, end - 1));
				numberOfBitsRequired -= (end - start);
				curr = end;
			}
		}
		curr = (curr + 1) % this->numberOfBits;
	} while (curr != currentBit && numberOfBitsRequired>0);
	return ret;
}

size_t Bitmap::count(int value)
{
	size_t c = 0;
	for (int i = 0; i < numberOfBits; i++)
		if (getBit(i) == value)
			c++;
	return c;
}

