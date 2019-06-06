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
	this->numberOfChars = std::ceil(numberOfBits / sizeof(char));

	this->numberOfBits = numberOfBits;
	this->bits = std::make_unique<char[]>(numberOfChars);
	std::memset(this->bits.get(), 0, numberOfChars);

	currentBit = 0;
}

int Bitmap::findNextFreeField()
{
	int oldBit = currentBit++;
	for (; currentBit != oldBit; currentBit = (currentBit+1)%numberOfBits) {
		int charIdx = currentBit / SIZE_OF_ONE;
		int bitIdx = currentBit % SIZE_OF_ONE;
		if (!((bits)[charIdx] & (1 << bitIdx))) {
			return currentBit;
		}
	}
	return -1;
}

int Bitmap::findExtentStart(size_t extentSizeInBytes)
{
	int numberOfBitsRequired = (int)std::ceil((double)extentSizeInBytes / sizeOfOneBit);
	if (numberOfBitsRequired > numberOfBits)
		return -1;//ne moze nikad imati vise nego sto ima...

	int curr = currentBit;
	do {
		if (getBit(curr) == 0) {
			int found = 1;
			for (int i = 1; ((i <= numberOfBitsRequired - 1) && (i+curr<numberOfBits)) ; i++) {
				if (getBit(curr + i) != 0)
					break;
				else
					found++;
			}
			if (found == numberOfBitsRequired)
				return curr;
		}
		curr = (curr + 1) % this->numberOfBits;
	} while (curr != currentBit);
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
			while (getBit(end) == 0 && (end - start + 1) <= numberOfBitsRequired)
				end++;
			if (end > start) {
				ret.push_back(std::make_pair(start, end - 1));
				numberOfBitsRequired -= (end - start);
				curr = end;
			}
		}
		curr++;
	} while (curr != currentBit && numberOfBitsRequired>0);
	return ret;
}

void Bitmap::setBit(int idx)
{
	if (idx >= numberOfBits)
		throw std::exception("Indeks van opsega!");
	else
		bits[idx / SIZE_OF_ONE] = (bits[idx / SIZE_OF_ONE] | (1 << (idx % SIZE_OF_ONE)));
}

void Bitmap::clearBit(int idx)
{
	if (idx >= numberOfBits)
		throw std::exception("Indeks van opsega!");
	else
		bits[idx / SIZE_OF_ONE] = (bits[idx / SIZE_OF_ONE] & (~(1 << (idx % SIZE_OF_ONE))));
}

int Bitmap::getBit(int idx) const
{
	if (idx >= numberOfBits)
		throw std::exception("Indeks izvan opsega!");
	else
		return (bits[idx / SIZE_OF_ONE] & (1 << (idx % SIZE_OF_ONE)))>>(idx%SIZE_OF_ONE);//shift desno da vrati 0/1
}

const std::unique_ptr<char[]>& Bitmap::getBits() const
{
	return bits;
}

