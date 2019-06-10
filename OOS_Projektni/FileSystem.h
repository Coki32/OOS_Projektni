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

	//std::vector<ListItem> rootEntries;

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
	size_t writeFile(const std::shared_ptr<Data>& data, const std::string& path, bool overwrite = false);


	//Creates the specified folder if the folder exists it prints out a message, otherwise returns iNode number for the folder it just created
	size_t mkdir(const std::string& folderName);

	//ls all the way from root, recursilve, prints all files
	void ls(size_t nodeID = 0, bool recursive = false, const std::string & prefix = "");

	//ls, from specified path, not-recursive by default
	void ls(const std::string& path, bool recursive = false);

	//Copies data from "from" into new file "to", does not overwrite the existing file returns the iNode ID of the file which it created
	size_t cp(const std::string& from, const std::string& to);

	//Creates an empty file with size 0, no blocks asociated
	size_t create(const std::string& path);


	//Writes from string "content" into the file pointed to by the "path" parameter. File must exist.
	size_t echo(const std::string& path, const std::string& content);


	//Prints out the content of the file pointed to by the "path" string onto (by default) the console output
	void cat(const std::string& path, std::ostream& os = std::cout);

	//Deletes the file with the specified iNode ID
	void deleteFile(size_t nodeID);

	//Deletes the file pointed to by the string "path", also removes it from the parent folder
	void deleteFile(const std::string& path);

	//Also deletes the file by the path, but it can remove folders and files
	void rm(const std::string& path, bool recursive = false);

	//reads data from the file pointed to by the specified nodeID
	//TODO: Move to private
	std::shared_ptr<Data> readFile(size_t nodeID);

	//reads data from the file pointed to by the specified string "path"
	std::shared_ptr<Data> readFile(const std::string& path);


	//moves the file from "from" to "to" paths
	void mv(const std::string& from, const std::string& to);

	//Renames the last path part from name "original", to name "newName"
	//eg rename("root/folderA/subfolder","nope") would rename "subfolder" to "nope"
	//eg rename("root/folderA/subfolder/file.a","nope") would rename "file.a" to "nope"
	void rename(const std::string& original, const std::string& newName);


	size_t getActualSize() const;
	size_t getDataSize() const;

	size_t getFreeSpace() const;

	size_t findIDByPath(const std::string& path);

private:

	void addFileToFolder(size_t folderID, size_t fileID, const std::string& filename);

	void saveFileList(size_t nodeId, const std::vector<ListItem>& list);
	std::vector<ListItem> loadFileList(size_t nodeId);

	std::shared_ptr<INode> loadNode(size_t nodeID);
	std::shared_ptr<Data> getNodeData(const std::shared_ptr<INode>& node);
	void writeNode(size_t nodeID, const std::shared_ptr<INode>& node);

	std::shared_ptr<Block> loadBlock(size_t blockID);
	//Upisuje 128 bajtova u taj blok, kao rezultat vraca sto je ostalo da se upise dalje
	std::shared_ptr<Data> writeBlock(size_t blockID, const std::shared_ptr<Data>& data);


	size_t writeFile(const std::shared_ptr<Data>& data);

	//writes to the specified node, overwriting it, no checks performed
	size_t writeFile(size_t nodeId, const std::shared_ptr<Data>& data, INode::TYPE tip = INode::TYPE::FILE);


	void saveBitmaps();

	std::streamoff calculateNodePos(size_t nodeID);
	std::streamoff calculateBlockPos(size_t blockID);


};

