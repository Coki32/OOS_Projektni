#include <exception>

#include "FileSystem.h"
#include "DiskList.h"
FileSystem::FileSystem()
{
	numberOfNodes = DEFAULT_NUMBER_OF_INODES;
	numberOfBlocks = DEFAULT_NUMBER_OF_BLOCKS;
	nodeBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfNodes, sizeof(INode)));
	blockBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfBlocks, sizeof(Block)));
	iNodes = std::make_unique<INode[]>(numberOfNodes);
	blocks = std::make_unique<Block[]>(numberOfBlocks);

	file = std::fstream(DEFAULT_FILENAME, std::fstream::out| std::fstream::in | std::fstream::binary | std::fstream::trunc);
	rootEntries = std::vector<ListItem>();
	rootEntries.push_back(ListItem(".", 0));
	rootEntries.push_back(ListItem("Neki Fajl", 1235));
	rootEntries.push_back(ListItem("Drugi fajl", 134));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	rootEntries.push_back(ListItem("NEMA LIMITA BURAAAZ", 129));
	writeBytes(0,DiskList::toData(rootEntries));
	if (file.is_open()) {
		file.write((char*)& numberOfNodes, sizeof(numberOfNodes));
		file.write((char*)& numberOfBlocks, sizeof(numberOfBlocks));

		file.write(nodeBitmap->getBits().get(), nodeBitmap->getActualSize());
		file.write(blockBitmap->getBits().get(), blockBitmap->getActualSize());
		file.write((char*)iNodes.get(), (std::streamsize)numberOfNodes*sizeof(INode));//BROJ U BAJTOVIMA!!!!!
		file.write((char*)blocks.get(), (std::streamsize)numberOfBlocks*sizeof(Block));
	}
}

FileSystem::FileSystem(const char* filename)
{
	this->file = std::fstream(filename, std::ios::binary | std::ios::in | std::ios::out);
	if (file.is_open()) {
		file.read((char*)& numberOfNodes, sizeof(numberOfNodes));
		file.read((char*)& numberOfBlocks, sizeof(numberOfBlocks));

		nodeBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfNodes, sizeof(INode)));
		blockBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfBlocks, sizeof(Block)));
		iNodes = std::make_unique<INode[]>(numberOfNodes);
		blocks = std::make_unique<Block[]>(numberOfBlocks);

		file.read(nodeBitmap->getBits().get(), numberOfNodes);
		file.read(blockBitmap->getBits().get(), numberOfBlocks);
		file.read((char*)iNodes.get(), (std::streamsize)numberOfNodes * sizeof(INode));
		file.read((char*)blocks.get(), (std::streamsize)numberOfBlocks * sizeof(Block));
	}
}

FileSystem::~FileSystem()
{
	if (file.is_open()) {
		file.flush();
		file.close();
	}
}

size_t FileSystem::writeBytes(size_t nodeId, const std::unique_ptr<Data>& data)
{
	if (nodeBitmap->getBit(nodeId) == 0) {
		nodeBitmap->setBit(nodeId);
		int blockIdx = blockBitmap->findExtentStart(data->length);
		if (blockIdx != -1) {
			int start, end;
			iNodes[nodeId].extentInfo[0][0] = start = blockIdx;
			iNodes[nodeId].extentInfo[0][1] = end = blockIdx + ((int)std::ceil(data->length / BLOCK_SIZE));
			const char* shiftingData = &data->data[0];
			int remainingLength = data->length;
			for (int i = start; i <= end; i++) {
				blockBitmap->setBit(i);
				memcpy_s(blocks[i].data, BLOCK_SIZE, shiftingData, remainingLength>=BLOCK_SIZE ? BLOCK_SIZE : remainingLength);
				shiftingData += BLOCK_SIZE;
				remainingLength -= BLOCK_SIZE;
			}
		}
	}
	else
	{
		std::cout << "Zauzet node, uzmi drugi!";
	}
	return data->length;
}

size_t FileSystem::getActualSize() const
{
	return nodeBitmap->getActualSize() + blockBitmap->getActualSize() + iNodes[0].getActualSize() * DEFAULT_NUMBER_OF_INODES + blocks[0].getActualSize() * DEFAULT_NUMBER_OF_BLOCKS;
}

size_t FileSystem::getDataSize() const
{
	return blocks[0].getActualSize() * DEFAULT_NUMBER_OF_BLOCKS;
}
