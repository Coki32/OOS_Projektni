#include <exception>
#include <algorithm>

#include "FileSystem.h"
#include "DiskList.h"
#include "Util.h"


FileSystem::FileSystem()
{
	numberOfNodes = DEFAULT_NUMBER_OF_INODES;
	numberOfBlocks = DEFAULT_NUMBER_OF_BLOCKS;
	nodeBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfNodes, sizeof(INode)));
	blockBitmap = std::unique_ptr<Bitmap>(new Bitmap(numberOfBlocks, sizeof(Block)));
	iNodes = std::make_unique<INode[]>(numberOfNodes);
	blocks = std::make_unique<Block[]>(numberOfBlocks);

	file = std::fstream(DEFAULT_FILENAME, std::fstream::out| std::fstream::in | std::fstream::binary | std::fstream::trunc);
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

		rootEntries = std::vector<ListItem>();
		rootEntries.push_back(ListItem("root", 0));
		saveFileList(0, rootEntries);

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


std::shared_ptr<Data> FileSystem::readData(size_t nodeID)
{
	auto data = std::make_shared<Data>(nullptr, 0);
	int nextNode = nodeID;
	size_t filesize = 0;
	do {
		auto node = loadNode(nextNode);
		if (filesize == 0)
			filesize = node->fileSize;
		data = data->append(getNodeData(node));
		nextNode = node->nextNode;
	} while (nextNode != (uint16_t)INode::NOT_SET);
	return data->takeNFromLeft(filesize);
}

std::shared_ptr<Data> FileSystem::readFile(size_t nodeID)
{
	return readData(nodeID);
}

size_t FileSystem::getActualSize() const
{
	return nodeBitmap->getActualSize() + blockBitmap->getActualSize() + iNodes[0].getActualSize() * numberOfNodes + blocks[0].getActualSize() * numberOfBlocks;
}

size_t FileSystem::getDataSize() const
{
	return blocks[0].getActualSize() * numberOfBlocks;
}

void FileSystem::saveFileList(size_t nodeId, const std::vector<ListItem>& list)
{
	if (nodeBitmap->getBit(nodeId))
		deleteFile(nodeId);
	writeFile(nodeId, DiskList::toData(list));
}

std::vector<ListItem> FileSystem::loadFileList(size_t nodeId)
{
	std::vector<ListItem> content;
	auto node = loadNode(nodeId);
	if (node->type == INode::TYPE::FOLDER) {
		auto data = getNodeData(node);
		content = DiskList::fromData(data);
	}
	return content;
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

//Write node samo treba da upise taj node na to mjesto, nista vise
void FileSystem::writeNode(size_t nodeID, const std::shared_ptr<INode>& node)
{
	nodeBitmap->setBit(nodeID);
	saveBitmaps();
	file.seekp(calculateNodePos(nodeID));
	file.write((char*)node.get(), sizeof(INode));
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
	if (data->length > 0) {
		file.seekp(calculateBlockPos(blockID));
		file.write(data->data.get(), data->length < BLOCK_SIZE ? data->length : BLOCK_SIZE);
		blockBitmap->setBit(blockID);
		saveBitmaps();
		return data->removeNFromLeft(BLOCK_SIZE);
	}
	else
		return std::make_shared<Data>(nullptr, 0);
}

void FileSystem::deleteFile(size_t nodeID)
{
	std::vector<uint16_t> nodesToRelease;
	std::vector<size_t> blocksToRelease;
	uint16_t nextNode = nodeID;
	do {
		auto node = loadNode(nextNode);
		nodesToRelease.push_back(nodeID);
		for (const auto& block : node->getBlocks())
			blocksToRelease.push_back(block);
		nextNode = node->nextNode;
	} while (nextNode != (uint16_t)INode::NOT_SET);
	if (nodesToRelease.size() > 0) {
		for (const auto& n : nodesToRelease)
			nodeBitmap->clearBit(n);
		nodeBitmap->setCurrent(*std::min_element(nodesToRelease.begin(), nodesToRelease.end()));
	}
	if (blocksToRelease.size() > 0) {
		for (const auto& b : blocksToRelease)
			blockBitmap->clearBit(b);
		blockBitmap->setCurrent(*std::min_element(blocksToRelease.begin(), blocksToRelease.end()));
	}
	saveBitmaps();
}

size_t FileSystem::writeFile(const std::shared_ptr<Data>& data)
{
	return writeFile(nodeBitmap->findNextFreeField(), data);
}

size_t FileSystem::writeFile(const std::shared_ptr<Data>& data, const std::string& path)
{
	auto parts = Util::stringSplit(path, '/');
	if (parts[0].compare("root") != 0)
	{
		std::cout << "Putanje moraju biti apsolutne, pocinju sa \"root/\"" << std::endl;
		return -1;
	}
	else {
		//nasao je root, trazi ono iza njega
		int next = 1;
		size_t parentIndex;
		auto startingFolder = loadFileList(0);//kreni od roota
		ListItem entry = ListItem("",-1);
		do {
			entry = Util::findByName(startingFolder, parts[next]);
			if (entry.nodeIndex > 0) {
				auto node = loadNode(entry.nodeIndex);
				if (node->type == INode::TYPE::FOLDER) {
					startingFolder = DiskList::fromData(getNodeData(node));
					parentIndex = entry.nodeIndex;
				}
				next++;
			}
			else
				break;
		} while (entry.nodeIndex!=-1);
		if (next == (parts.size() - 1)) {
			auto nodeId = writeFile(data);
			startingFolder.push_back(ListItem(parts[next], nodeId));
			saveFileList(parentIndex, startingFolder);
			return nodeId;
		}
		else
			std::cout << "Nema toga!";
	}
	return 1;
}

size_t FileSystem::mkdir(const std::string& folderName)
{
	auto parts = Util::stringSplit(folderName, '/');
	int next = 0;
	auto startingFolder = rootEntries;
	size_t parentFolder = 0;
	ListItem entry = ListItem("", -1);
	do {
		entry = Util::findByName(startingFolder, parts[next]);
		if (entry.nodeIndex >= 0) {
			auto node = loadNode(entry.nodeIndex);
			if (node->type == INode::TYPE::FOLDER) {
				startingFolder = DiskList::fromData(getNodeData(node));
				parentFolder = entry.nodeIndex;
			}
			next++;
		}
		else
			break;
	} while (entry.nodeIndex != -1);
	if (next == (parts.size() - 1)) {
		auto folderID = nodeBitmap->findNextFreeField();
		auto folderNode = std::make_shared<INode>();
		folderNode->type = INode::TYPE::FOLDER;
		writeNode(folderID, folderNode);
		startingFolder.push_back(ListItem(parts[next], folderID));
		saveFileList(parentFolder, startingFolder);
		return folderID;
	}
	else
		std::cout << "Nema toga!";
}

size_t FileSystem::writeFile(size_t nodeId, const std::shared_ptr<Data>& data)
{
	auto extents = blockBitmap->findExtentStart_v(data->length);
	auto modifiableData = std::make_shared<Data>(data->data.get(), data->length);
	
	std::shared_ptr<INode> newNode;
	size_t startNodePos = nodeId;
	nodeBitmap->setBit(nodeId);
	newNode = std::make_shared<INode>(data->length);

	while (extents.size() > 0) {
		for (int i = 0; i < 6; i++)
		{
			auto first = extents.front();
			extents.erase(extents.begin());
			newNode->usesExtents = 1;
			newNode->extentInfo[i][0] = first.first;
			newNode->extentInfo[i][1] = first.second;
			for (int blockId = first.first; blockId <= first.second; blockId++) {
				modifiableData = writeBlock(blockId, modifiableData);
				if (modifiableData->length == 0)
				{
					writeNode(nodeId, newNode);
					saveBitmaps();
					return startNodePos;
				}
			}
		}
		writeNode(nodeId, newNode);
		if (extents.size() > 0) {//ako ih ima jos za upisati
			nodeId = nodeBitmap->findNextFreeField();
			nodeBitmap->setBit(nodeId);
			newNode->nextNode = (uint16_t)nodeId;
			newNode = std::make_shared<INode>(modifiableData->length);
		}
	}
	//ovo bi trebalo biti unreachable, al ono, ok...
	saveBitmaps();
	return startNodePos;
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

