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

	const static size_t SIZE_OF_ONE = sizeof(char);
public:
	Bitmap(int numberOfBits, size_t sizeOfOneBit);

	size_t findNextFreeField();
	std::vector<std::pair<int,int>> findExtentStart_v(size_t extentSizeInBytes);

	size_t count(int value);

	inline void setBit(int idx)
	{
		if (idx >= numberOfBits)
			throw std::exception("Indeks van opsega!");
		else
			bits[idx / SIZE_OF_ONE] = (bits[idx / SIZE_OF_ONE] | (1 << (idx % SIZE_OF_ONE)));
	}

	inline void clearBit(int idx)
	{
		if (idx >= numberOfBits)
			throw std::exception("Indeks van opsega!");
		else
			bits[idx / SIZE_OF_ONE] = (bits[idx / SIZE_OF_ONE] & (~(1 << (idx % SIZE_OF_ONE))));
	}

	inline int getBit(int idx) const
	{
		if (idx >= numberOfBits)
			throw std::exception("Indeks izvan opsega!");
		else
			return (bits[idx / SIZE_OF_ONE] & (1 << (idx % SIZE_OF_ONE))) >> (idx % SIZE_OF_ONE);//shift desno da vrati 0/1
	}
	
	inline void setCurrent(int current) {
		if (current >= numberOfBits)
			throw std::exception("Ne mozes postaviti current na nesto van niza!");
		currentBit = current; 
	}

	inline const std::unique_ptr<char[]>& getBits() const { return bits; }

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

