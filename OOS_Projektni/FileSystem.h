#pragma once
#include <iostream>
#include <fstream>
#include <vector> 
#include <functional>

#include "Bitmap.h"
#include "Blok.h"
#include "INode.h"
#include "ListItem.h"

#include "File.h"

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


	std::shared_ptr<Data> readData(size_t nodeID);

	//writes to first free node
	size_t writeFile(const std::shared_ptr<Data>& data);
	size_t writeFile(const std::shared_ptr<Data>& data, const std::string& path);


	//mkdir
	size_t mkdir(const std::string& folderName);


/*
	1 - Mark all nodes as free
	2 - Mark all blocks as free
*/
	void deleteFile(size_t nodeID);


	std::shared_ptr<Data> readFile(size_t nodeID);

	size_t getActualSize() const;
	size_t getDataSize() const;

private:

	void saveFileList(size_t nodeId, const std::vector<ListItem>& list);
	std::vector<ListItem> loadFileList(size_t nodeId);

	std::shared_ptr<INode> loadNode(size_t nodeID);
	std::shared_ptr<Data> getNodeData(const std::shared_ptr<INode>& node);
	void writeNode(size_t nodeID, const std::shared_ptr<INode>& node);

	std::shared_ptr<Block> loadBlock(size_t blockID);
	//Upisuje 128 bajtova u taj blok, kao rezultat vraca sto je ostalo da se upise dalje
	std::shared_ptr<Data> writeBlock(size_t blockID, const std::shared_ptr<Data>& data);

	
	//writes to the specified node, overwriting it, no checks performed
	size_t writeFile(size_t nodeId, const std::shared_ptr<Data>& data);



	void saveBitmaps();

	std::streamoff calculateNodePos(size_t nodeID);
	std::streamoff calculateBlockPos(size_t blockID);


};

