#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "DiskList.h"

struct BinaryListEntry {
	int strLen;
	const char* str;
	int nodeIdx;
	BinaryListEntry() : strLen(0), str(nullptr), nodeIdx(0){}
	BinaryListEntry(const std::string& str, int nodeIdx) {
		this->strLen = str.length()+1;
		this->str = str.c_str();
		this->nodeIdx = nodeIdx;
	}
	int getActualSize() const {
		return sizeof(int) * 2 + strLen;
	}

	std::unique_ptr<char[]> toPlainBinary() const {
		std::unique_ptr<char[]> ret = std::make_unique<char[]>(getActualSize());
		*((int*)ret.get()) = strLen;
		auto a = ret[0];
		memcpy_s(&ret[0] + sizeof(int), getActualSize() - sizeof(int), this->str, this->strLen);
		*((int*)(ret.get() + sizeof(int) + sizeof(char)*strLen)) = nodeIdx;
		return ret;
	}

	ListItem toFancyItem() {
		return ListItem(std::string(str), nodeIdx);
	}

	static BinaryListEntry fromBinary(char* data) {
		BinaryListEntry e;
		e.strLen = *(int*)data;
		data += sizeof(e.strLen);
		e.str = data;
		data += e.strLen;
		e.nodeIdx = *(int*)data;
		return e;
	}
};

std::shared_ptr<Data> DiskList::toData(const std::vector<ListItem>& items)
{
	size_t count = items.size();
	size_t totalSize = 0, complete = 0;

	std::shared_ptr<Data> result = std::make_unique<Data>(nullptr,0);
	std::vector<BinaryListEntry> entries;

	for(const auto& li : items){
		BinaryListEntry entry = BinaryListEntry(li.name, li.nodeIndex);
		entries.push_back(entry);
		totalSize += entry.getActualSize();
	}
	//dodaj jos mjesta za broj elemenata liste
	totalSize += sizeof(count);
	//alociraj data za ukupnu velicinu
	result->data = std::unique_ptr<char[]>(new char[totalSize]);
	//"upisi" broj elemenata na pocetak
	memcpy_s(&result->data[0], totalSize + sizeof(int), &count, sizeof(count));
	//reci da je taj dio "gotov"
	complete += sizeof(count);

	//iskopiraj jedan po jedan entry u listu
	for(const auto& entry : entries){
		memcpy_s((char*)&result->data[0] + complete, totalSize, entry.toPlainBinary().get(), entry.getActualSize());
		complete += entry.getActualSize();
	}
	//potvrdi da je sve to tacno te veclicine
	_ASSERT(complete == totalSize);
	result->length = totalSize;//postavi velicinu
	return result;//vrati Data pokazivac
}

std::vector<ListItem> DiskList::fromData(const std::shared_ptr<Data>& data)
{
	std::vector<ListItem> items;
	if (data->length > 0) {
		size_t numberOfElements;
		char* binary = &data->data[0];
		numberOfElements = *(int*)binary;//prvi podatak je int koliko ih je
		binary += sizeof(numberOfElements);
		for (size_t i = 0; i < numberOfElements; i++) {
			BinaryListEntry entry = BinaryListEntry::fromBinary(binary);
			items.push_back(entry.toFancyItem());
			binary += entry.getActualSize();
		}
	}
	return items;
}
