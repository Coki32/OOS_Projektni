#include <sstream>
#include <algorithm>

#include "Util.h"

ListItem Util::findByName(const std::vector<ListItem>& fileList, const std::string& name)
{
	auto it = std::find_if(fileList.begin(), fileList.end(), [&name](const ListItem& li) {
		return li.name.compare(name) == 0;
	});
	if (it == fileList.end())
		return ListItem("", -1);
	else
		return *it;
}

bool Util::stringContains(const std::string& string, char c)
{
	return std::find(string.begin(), string.end(), c) != string.end();
}

std::vector<std::string> Util::stringSplit(const std::string& str, char delimiter)
{
	std::vector<std::string> parts;
	std::string part;
	std::istringstream stringStream(str);
	while (std::getline(stringStream, part, delimiter))
		parts.push_back(part);
	return parts;
}
