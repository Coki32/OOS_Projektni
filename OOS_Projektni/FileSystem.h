#pragma once
#include <iostream>
#include <fstream>
#include <vector> 

#include "Bitmap.h"
#include "Blok.h"
#include "INode.h"
#include "ListItem.h"

#define DEFAULT_FILENAME "defaultName.ext4nt"
class FileSystem
{

	std::unique_ptr<Bitmap> nodeBitmap;
	std::unique_ptr<Bitmap> blockBitmap;
	std::unique_ptr<INode[]> iNodes;
	std::unique_ptr<Block[]> blocks;

	std::vector<ListItem> rootEntries;

	std::fstream file;

	size_t numberOfNodes, numberOfBlocks;

public:

	const static size_t DEFAULT_NUMBER_OF_INODES = 13067;
	const static size_t DEFAULT_NUMBER_OF_BLOCKS = 156796;
	FileSystem();
	FileSystem(const char* file);

	~FileSystem();

	size_t writeBytes(size_t nodeId, const std::unique_ptr<Data>& data);

	size_t getActualSize() const;
	size_t getDataSize() const;
};

