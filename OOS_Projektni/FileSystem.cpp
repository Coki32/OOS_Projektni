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
		nodesOffset = file.tellp();
		file.write((char*)iNodes.get(), (std::streamsize)numberOfNodes*sizeof(INode));//BROJ U BAJTOVIMA!!!!!
		blocksOffset = file.tellp();
		file.write((char*)blocks.get(), (std::streamsize)numberOfBlocks*sizeof(Block));
		std::cout << nodesOffset << std::endl << blocksOffset << std::endl;

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

		file.read(nodeBitmap->getBits().get(), numberOfNodes);
		file.read(blockBitmap->getBits().get(), numberOfBlocks);
		nodesOffset = file.tellg();
		blocksOffset = nodesOffset + std::streampos((long long)numberOfNodes * sizeof(INode));

		auto rootList = readData(0);
		rootEntries = DiskList::fromData(rootList);
		for (const auto& li : rootEntries) {
			std::cout << li.name << " - " << li.nodeIndex << std::endl;
		}
	}
}

FileSystem::~FileSystem()
{
	if (file.is_open()) {
		file.flush();
		file.close();
	}
}

size_t FileSystem::writeBytes(size_t nodeId, const std::shared_ptr<Data>& data)
{
	if (nodeBitmap->getBit(nodeId) == 0) {
		nodeBitmap->setBit(nodeId);
		int blockIdx = blockBitmap->findExtentStart(data->length);
		if (blockIdx != -1) {
			int start, end;
			iNodes[nodeId].usesExtents = 1;
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

std::shared_ptr<Data> FileSystem::readData(size_t nodeID)
{
	auto data = std::make_shared<Data>(nullptr, 0);
	int nextNode = nodeID;
	do {
		auto node = loadNode(nextNode);
		data = data->append(getNodeData(node));
		nextNode = node->nextNode;
	} while (nextNode != 0);
	return data;
}

size_t FileSystem::getActualSize() const
{
	return nodeBitmap->getActualSize() + blockBitmap->getActualSize() + iNodes[0].getActualSize() * numberOfNodes + blocks[0].getActualSize() * numberOfBlocks;
}

size_t FileSystem::getDataSize() const
{
	return blocks[0].getActualSize() * numberOfBlocks;
}

std::shared_ptr<INode> FileSystem::loadNode(size_t nodeID)
{
	std::shared_ptr<INode> node = std::make_shared<INode>();
	file.seekg(calculateNodePos(nodeID));
	file.read((char*)node.get(), sizeof(INode));
	return node;
}

std::shared_ptr<Data> FileSystem::getNodeData(const std::shared_ptr<INode>& node)
{
	auto data = std::make_shared<Data>(nullptr, 0);
	for (const auto& blockID : node->getBlocks()) {
		data = data->append(loadBlock(blockID)->getData());
	}
	return data;
}

void FileSystem::writeNode(size_t nodeID, const std::shared_ptr<INode>& node)
{
	//nema provjere jer je mozda update node-a
	size_t nextNode = nodeID;
	do {

	} while (nextNode != 0);
}

std::shared_ptr<Block> FileSystem::loadBlock(size_t blockID)
{
	auto block = std::make_shared<Block>();
	file.seekg(calculateBlockPos(blockID));
	file.read((char*)block.get(), sizeof(Block));
	return block;
}

std::shared_ptr<Data> FileSystem::writeBlock(size_t blockID, const std::shared_ptr<Data>& data)
{
	file.seekp(blocksOffset + std::streampos((long long)blockID * BLOCK_SIZE));
	file.write(data->data.get(), data->length<BLOCK_SIZE ? data->length : BLOCK_SIZE);
	return data->removeNFromLeft(BLOCK_SIZE);
}

void FileSystem::deleteFile(size_t nodeID)
{
	auto node = loadNode(nodeID);
	for (const auto& blockID : node->getBlocks()) {
		if (blockID != 0)
			blockBitmap->clearBit(blockID);
	}
	nodeBitmap->clearBit(nodeID);
	saveBitmaps();
}



void FileSystem::saveBitmaps()
{
	//mislim, ko i file.seekp(8) ali ako neko nesto promijeni....
	file.seekp(0+sizeof(numberOfBlocks)+sizeof(numberOfNodes));
	file.write(nodeBitmap->getBits().get(), nodeBitmap->getActualSize());
	file.write(blockBitmap->getBits().get(), blockBitmap->getActualSize());
}



static std::streamoff actualPosition(size_t idx, size_t sizeOfOne, std::streampos start)
{
	return start + std::streampos((long long)idx * sizeOfOne);
}

std::streamoff FileSystem::calculateNodePos(size_t nodeID)
{
	return actualPosition(nodeID, sizeof(INode), nodesOffset);
}

std::streamoff FileSystem::calculateBlockPos(size_t blockID)
{
	return actualPosition(blockID, sizeof(Block), blocksOffset);
}

