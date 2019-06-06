#pragma once
#include <iostream>
#include <fstream>
#include <vector> 
#include <functional>

#include "Bitmap.h"
#include "Blok.h"
#include "INode.h"
#include "ListItem.h"

#define DEFAULT_FILENAME "defaultName.ext4nt"
class FileSystem
{

	std::streampos nodesOffset, blocksOffset;

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

	size_t writeBytes(size_t nodeId, const std::shared_ptr<Data>& data);

	std::shared_ptr<Data> readData(size_t nodeID);


	size_t getActualSize() const;
	size_t getDataSize() const;

private:
	std::shared_ptr<INode> loadNode(size_t nodeID);
	std::shared_ptr<Data> getNodeData(const std::shared_ptr<INode>& node);
	void writeNode(size_t nodeID, const std::shared_ptr<INode>& node);

	std::shared_ptr<Block> loadBlock(size_t blockID);
	//Upisuje 128 bajtova u taj blok, kao rezultat vraca sto je ostalo da se upise dalje
	std::shared_ptr<Data> writeBlock(size_t blockID, const std::shared_ptr<Data>& data);

	//Brise fajl tako sto
	/*
		1 - oznaci node da je slobodan
		2 - oznaci sve blokove node-a da su slobodni
	*/
	void deleteFile(size_t nodeID);
	
	//Upisuje sadrzaj u neki iNode, vraca broj iNode-a
	size_t writeFile(const std::shared_ptr<Data>& data);


	void saveBitmaps();

	std::streamoff calculateNodePos(size_t nodeID);
	std::streamoff calculateBlockPos(size_t blockID);


};

