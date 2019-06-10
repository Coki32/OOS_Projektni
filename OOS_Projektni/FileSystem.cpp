#include <exception>
#include <algorithm>
#include <string_view>

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

		std::vector<ListItem> root;
		root.push_back(ListItem("root", 1));
		saveFileList(0, root);
		std::vector<ListItem> rootEntries;
		saveFileList(1, rootEntries);
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


void FileSystem::deleteFile(const std::string& path)
{
	auto parent = Util::parentInPath(path);
	auto file = Util::terminalPath(path);
	size_t parentID = -1;
	size_t fileID = -1;
	try {
		parentID = findIDByPath(parent);
	}
	catch (std::exception& ex) {}
	if (parentID == -1) {
		std::cout << "Putanja nije dobro zadata!" << std::endl;
		return;
	}
	auto parentFileList = loadFileList(parentID);
	auto searchResult = Util::findByName(parentFileList, file);
	if (searchResult.nodeIndex == -1) {
		std::cout << "Trazeni fajl ne postoji u datom folderu!" << std::endl;
		return;
	}
	deleteFile(searchResult.nodeIndex);
	parentFileList.erase(std::find_if(
		parentFileList.begin(),
		parentFileList.end(),
		[&file](const ListItem& li) {return li.name.compare(file)==0; }));
	saveFileList(parentID, parentFileList);
	
}

void FileSystem::rm(const std::string& path, bool recursive)
{
	size_t nodeID = -1;
	try {
		nodeID = findIDByPath(path);
	}
	catch (std::exception& ex) {}
	if (nodeID == -1) {
		std::cout << "Data putanja ne postoji!" << std::endl;
		return;
	}
	auto node = loadNode(nodeID);
	if (node->type == INode::TYPE::FOLDER) {
		auto files = loadFileList(nodeID);
		if (!recursive && files.size() > 0) {
			std::cout << "Za brisanje foldera sa sadrzajem koristi -r opciju za rekurzivno brisanje!" << std::endl;
			return;
		}
		else if (recursive && files.size() > 0) {
			for (const auto& li : files) {
				auto fileNode = loadNode(li.nodeIndex);
				if (fileNode->type == INode::TYPE::FILE)
					rm(path + "/" + li.name, false);
				else
					rm(path + "/" + li.name, true);
			}
			//obrisi folder iz parent foldera
			auto parentID = findIDByPath(Util::parentInPath(path));
			auto parentList = loadFileList(parentID);
			parentList.erase(std::find_if(parentList.begin(), parentList.end(), [&nodeID](const ListItem& li) { return li.nodeIndex == nodeID; }));
			saveFileList(parentID, parentList);
			deleteFile(nodeID);//Oslobodi njegov prostor
		}
	}
	else {//ako je fajl, pozovi onu obicnu za brisanje fajla
		deleteFile(path);
	}
}

std::shared_ptr<Data> FileSystem::readFile(size_t nodeID)
{
	return readData(nodeID);
}

std::shared_ptr<Data> FileSystem::readFile(const std::string& path)
{
	size_t nodeID = -1;
	try {
		nodeID = findIDByPath(path);
	}
	catch (std::exception& ex) {}
	if (nodeID == -1) {
		std::cout << "Fajl ne postoji na trazenoj putanji!" << std::endl;
		return std::make_shared<Data>(nullptr, 0);
	}
	if (loadNode(nodeID)->type == INode::TYPE::FOLDER){
		std::cout << "Ne mozes citati iz foldera!" << std::endl;
		return std::make_shared<Data>(nullptr, 0);
	}
	return readFile(nodeID);
}

void FileSystem::mv(const std::string& from, const std::string& to)
{
	size_t fromID = -1, toID = -1;
	try {
		fromID = findIDByPath(Util::parentInPath(from));
		toID = findIDByPath(to);
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return;
	}
	if (fromID == -1 || toID == -1) {
		std::cout << "Putanje nisu ispravne!\nPrva putanja mora biti do datoteke, druga do foldera!" << std::endl;
		return;
	}
	auto fileList = loadFileList(fromID);
	auto sourceName = Util::terminalPath(from);
	auto searchResult = Util::findByName(fileList, sourceName);
	if (searchResult.nodeIndex == -1) {
		std::cout << "Trazena datoteka ne postoji u navedenom folderu!" << std::endl;
		return;
	}
	if (loadNode(toID)->type != INode::TYPE::FOLDER) {
		std::cout << "Odrediste mora biti folder!" << std::endl;
		return;
	}
	auto sourceList = loadFileList(toID);
	if (Util::findByName(sourceList, searchResult.name).nodeIndex == -1) {
		sourceList.push_back(searchResult);
		fileList.erase(std::find_if(fileList.begin(), fileList.end(),
			[&sourceName](const ListItem& li) { return li.name.compare(sourceName) == 0; }
		));
		saveFileList(fromID, fileList);
		saveFileList(toID, sourceList);
	}
	else {
		std::cout << "Navedeno ime vec postoji u folderu!" << std::endl;
	}
}

void FileSystem::rename(const std::string& original, const std::string& newName)
{
	size_t parentID = -1, fileID;
	if (Util::stringSplit(newName, '/').size() > 1) {
		std::cout << "Novo ime ne moze biti putanja! Za to koristi mv(...)" << std::endl;
		return;
	}
	try {
		parentID = findIDByPath(Util::parentInPath(original));
	}
	catch (std::exception& ex) {}
	if (parentID == -1) {
		std::cout << "Putanja nije ispravna!" << std::endl;
		return;
	}
	auto parentFileList = loadFileList(parentID);
	if (Util::findByName(parentFileList, newName).nodeIndex != -1) {
		std::cout << "Trazeno ime vec postoji u datom folderu!" << std::endl;
		return;
	}
	auto oldFilename = Util::terminalPath(original);
	auto replacingIt = std::find_if(parentFileList.begin(), parentFileList.end(),
		[&oldFilename](const ListItem& li) { return li.name.compare(oldFilename) == 0; });
	replacingIt->name = newName;
	saveFileList(parentID, parentFileList);
}

size_t FileSystem::getActualSize() const
{
	return nodeBitmap->getActualSize() + blockBitmap->getActualSize() + iNodes[0].getActualSize() * numberOfNodes + blocks[0].getActualSize() * numberOfBlocks;
}

size_t FileSystem::getDataSize() const
{
	return blocks[0].getActualSize() * numberOfBlocks;
}

size_t FileSystem::getFreeSpace() const
{
	return blockBitmap->count(0) * BLOCK_SIZE;
}

void FileSystem::addFileToFolder(size_t folderID, size_t fileID, const std::string& filename)
{
	auto folderList = loadFileList(folderID);
	folderList.push_back(ListItem(filename, fileID));
	saveFileList(folderID, folderList);
}

void FileSystem::saveFileList(size_t nodeId, const std::vector<ListItem>& list)
{
	if (nodeBitmap->getBit(nodeId))
		deleteFile(nodeId);
	writeFile(nodeId, DiskList::toData(list),INode::TYPE::FOLDER);

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
		nodesToRelease.push_back(nextNode);
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

size_t FileSystem::writeFile(const std::shared_ptr<Data>& data, const std::string& path, bool overwrite)
{
	auto parts = Util::stringSplit(path, '/');
	if (parts[0].compare("root") != 0)
	{
		std::cout << "Putanje moraju biti apsolutne, pocinju sa \"root/\"" << std::endl;
		return -1;
	}
	//Ako je putanja tipa root/fajl1, root/fajl.ext itd
	else if (parts.size() == 2) {
		auto rootFileList = loadFileList(1);
		auto searchResult = Util::findByName(rootFileList, parts[1]);
		size_t fileID = searchResult.nodeIndex;
		if (searchResult.nodeIndex != -1 && overwrite) {
			deleteFile(searchResult.nodeIndex);
			writeFile(searchResult.nodeIndex, data);
		}
		else if (searchResult.nodeIndex != -1 && !overwrite) {
			std::cout << "Fajl sa istim imenom vec postoji na datoj putanji!" << std::endl;
			return -1;
		}
		else {
			fileID = writeFile(data);
			rootFileList.push_back(ListItem(parts[1], fileID));
			saveFileList(1, rootFileList);
		}
		//write file uradi sve ostalo
		return fileID;
	}
	//inace ima vise dijelova tipa root/folderA/folderB/fajl ili root/A/B/f.ext
	else {
		//pocne od root-a
		size_t currentFolderID = 0;
		auto currentFileList = loadFileList(currentFolderID);
		for (auto it = parts.begin(); it <= parts.end() - 2; ++it) {
			auto nextItem = Util::findByName(currentFileList, *it);
			if (nextItem.nodeIndex == -1) {
				std::cout << "Folder " << *it << " ne postoji na zadatoj putanji!" << std::endl;
				return -1;
			}
			else {
				auto node = loadNode(nextItem.nodeIndex);
				if (node->type != INode::TYPE::FOLDER) {
					std::cout << *it << " nije folder!" << std::endl;
					return -1;
				}
				else
					currentFileList = loadFileList(currentFolderID=nextItem.nodeIndex);
			}
		}//nakon for-a, nasao je taj folder, njegov filelist je trenutno ucitan
		//provjeri postoji li taj fajl vec u folderu
		auto searchResult = Util::findByName(currentFileList, *(parts.end() - 1));
		if (searchResult.nodeIndex != -1 && !overwrite) {
			std::cout << "Fajl sa istim imenom vec postoji na toj putanji!" << std::endl;
		}
		else {
			size_t fileID;
			if (searchResult.nodeIndex != -1 && overwrite) {//Ako je overwrite obrisi ga, pa prepisi
				deleteFile(searchResult.nodeIndex);
				fileID = writeFile(searchResult.nodeIndex, data);
				auto it = std::find_if(currentFileList.begin(), currentFileList.end(),
					[&parts](const ListItem& li) { return li.name.compare(*(parts.end() - 1)) == 0;});
				it->nodeIndex = fileID;
			}
			else {
				fileID = writeFile(data);
				currentFileList.push_back(ListItem(*(parts.end() - 1), fileID));
			}
			saveFileList(currentFolderID, currentFileList);
			return fileID;
		}
	}
	return -1;
}

size_t FileSystem::mkdir(const std::string& folderName)
{
	try {
		if (findIDByPath(folderName) != -1)
		{
			std::cout << "Folder vec postoji!" << std::endl;
			return -1;
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return -1;
	}
	std::string parent = Util::parentInPath(folderName);
	std::string newOne = Util::terminalPath(folderName);
	auto parentID = findIDByPath(parent);
	if (parentID == -1) {
		std::cout << "Putanja " << parent << " ne postoji!" << std::endl;
		return -1;
	}
	auto parentList = loadFileList(parentID);
	if (Util::findByName(parentList, newOne).nodeIndex != -1) {
		std::cout << "Folder vec postoji na zadatoj putanji!" << std::endl;
	}
	else {
		auto newfolderID = nodeBitmap->findNextFreeField();
		auto newfolderNode = std::make_shared<INode>();
		std::vector<ListItem> emptyList;
		saveFileList(newfolderID, emptyList);
		parentList.push_back(ListItem(newOne, newfolderID));
		saveFileList(parentID, parentList);
		return newfolderID;
	}
}

void FileSystem::ls(size_t nodeID, bool recursive, const std::string& prefix)
{
	auto node = loadNode(nodeID);
	if (node->type == INode::TYPE::FOLDER) {
		if (!recursive) {
			auto list = loadFileList(nodeID);
			for (const auto& li : list) {
				std::cout << prefix << li.name << "("<<li.nodeIndex<<")" << std::endl;
			}
		}
		else {
			auto list = loadFileList(nodeID);
			for (const auto& li : list) {
				std::cout << prefix << li.name << "(" << li.nodeIndex << ")" << std::endl;
				ls(li.nodeIndex, recursive, prefix + "\t");
			}
		}
	}
}

void FileSystem::ls(const std::string& path, bool recursive)
{
	size_t pathID = -1;
	try {
		pathID = path.length() == 0 ? 0 : findIDByPath(path);
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return;
	}
	if (pathID == -1) {
		std::cout << "Putanja " << path << " ne postoji!" << std::endl;
	}
	else
		ls(pathID, recursive);
}

size_t FileSystem::cp(const std::string& from, const std::string& to)
{
	try {
		auto fromID = findIDByPath(Util::parentInPath(from));
		if (fromID == -1) {
			std::cout << "Zadati folder ne postoji!" << std::endl;
			return -1;
		}
		auto fileList = loadFileList(fromID);
		auto fileID = Util::findByName(fileList,Util::terminalPath(from));
		if (fileID.nodeIndex == -1) {
			std::cout << "Zadati fajl ne postoji u datom folderu!" << std::endl;
			return -1;
		}
		if (loadNode(fileID.nodeIndex)->type != INode::TYPE::FILE) {
			std::cout << "Putanja mora zavrsavati fajlom!" << std::endl;
		}
		auto res = writeFile(readData(fileID.nodeIndex), to, false);
		if (res == -1) {
			std::cout << "Fajl sa istim imenom vec postoji u odredistu!" << std::endl;
		}
		return res;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}

size_t FileSystem::create(const std::string& path)
{
	auto parent = Util::parentInPath(path);
	auto filename = Util::terminalPath(path);
	try {
		size_t folderID = -1;
		try {
			folderID = findIDByPath(parent);
		}
		catch (std::exception& ex) {}
		if (folderID == -1) {
			std::cout << "Putanja ne postoji!" << std::endl;
			return -1;
		}
		auto folderFileList = loadFileList(folderID);
		auto searchResult = Util::findByName(folderFileList, filename);
		if (searchResult.nodeIndex != -1) {
			std::cout << "Fajl vec postoji na zadatoj putanji!" << std::endl;
			return -1;
		}
		auto newFileID = nodeBitmap->findNextFreeField();
		auto newFileNode = std::make_shared<INode>();
		writeNode(newFileID, newFileNode);
		folderFileList.push_back(ListItem(filename, newFileID));
		saveFileList(folderID, folderFileList);
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}

size_t FileSystem::echo(const std::string& path, const std::string& content)
{
	size_t nodeID = -1;
	try {
		nodeID = findIDByPath(path);
	}
	catch (std::exception& ex) {}
	if (nodeID == -1) {
		std::cout << "Ne mogu da nadjem fajl " << path << std::endl;
		return -1;
	}
	auto node = loadNode(nodeID);
	if (node->type == INode::TYPE::FOLDER) {
		std::cout << "Putanja je do foldera!" << std::endl;
		return -1;
	}
	auto data = std::make_shared<Data>(content.c_str(), content.length());
	//prvo ga obrisi pa upisi opet, da ne zauzima jos blokova....
	deleteFile(nodeID);
	return writeFile(nodeID, data);
}

void FileSystem::cat(const std::string& path, std::ostream& os)
{
	size_t nodeID = -1;
	try {
		nodeID = findIDByPath(path);
	}
	catch(std::exception& ex){}
	if (nodeID == -1) {
		std::cout << "Putanja nije ispravna!" << std::endl;
		return;
	}
	auto node = loadNode(nodeID);
	if (loadNode(nodeID)->type == INode::TYPE::FOLDER) {
		std::cout << "Putanja je do foldera! (ne mozes echo folder!)" << std::endl;
	}
	auto data = getNodeData(node);
	//string_view je tu da ogranici duzinu da ne predje slucajno sto ne treba!
	os << std::string_view(data->data.get(), data->length) << std::endl;
}

size_t FileSystem::writeFile(size_t nodeId, const std::shared_ptr<Data>& data, INode::TYPE tip)
{
	auto extents = blockBitmap->findExtentStart_v(data->length);
	auto modifiableData = std::make_shared<Data>(data->data.get(), data->length);
	
	std::shared_ptr<INode> newNode;
	size_t startNodePos = nodeId;
	nodeBitmap->setBit(nodeId);
	newNode = std::make_shared<INode>(data->length);
	newNode->type = tip;
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

size_t FileSystem::findIDByPath(const std::string& path)
{
	auto parts = Util::stringSplit(path, '/');
	size_t folderID = 0;
	auto nextPart = parts.begin();
	do {
		if (loadNode(folderID)->type == INode::TYPE::FILE) {
			//U vecini slucajeva ovo nije bitno, mora se paziti samo kod nekih poziva
			//zato se kod vecine koristi prazan try-catch blok, osim kod mkdir i cp npr
			std::cout << "Putaja ne zavrsava fajlom nego ih sadrzi!" << std::endl;
			throw std::exception("Putanja ne zavrsava fajlom!");
		}
		
		auto currentList = loadFileList(folderID);
		auto nextListItem = Util::findByName(currentList, *nextPart);
		if (nextListItem.nodeIndex == -1) {
			return -1;
		}
		else {
			folderID = nextListItem.nodeIndex;
		}
		++nextPart;
	} while (nextPart != parts.end());
	return folderID;
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

