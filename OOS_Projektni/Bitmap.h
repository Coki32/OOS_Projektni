#pragma once
#include <memory>
#include <iostream>
#include <vector>

class Bitmap
{

	std::unique_ptr<char[]> bits;
	int numberOfBits;
	int currentBit;
	int numberOfChars;
	size_t sizeOfOneBit;
public:
	Bitmap(int numberOfBits, size_t sizeOfOneBit);

	int findNextFreeField();
	int findExtentStart(size_t extentSizeInBytes);
	std::vector<std::pair<int,int>> findExtentStart_v(size_t extentSizeInBytes);

	void setBit(int idx);
	void clearBit(int idx);
	int getBit(int idx) const;
	

	const std::unique_ptr<char[]>& getBits() const;

	inline size_t getActualSize() const {
		return numberOfChars * sizeof(char);
	}
#ifdef DEBUG
	friend std::ostream& operator<<(std::ostream& os, const Bitmap& bmp) {
		for (int i = 0; i < bmp.numberOfBits; i++)
			os << bmp.getBit(i) << " ";
		return os;
	}
#endif // DEBUG

};

