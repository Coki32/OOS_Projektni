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

std::string Util::parentInPath(const std::string& path)
{
	std::string parent;
	auto parts = Util::stringSplit(path, '/');
	int next = 0;
	parts.erase(parts.end() - 1);
	for (const auto& p : parts)
		parent += p + "/";
	return parent;
}

std::string Util::terminalPath(const std::string& path)
{
	return *(Util::stringSplit(path, '/').end() - 1);
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

std::vector<std::string> Util::stringsAfter(const std::vector<std::string>& input, int n)
{
	auto copy = std::vector(input);
	copy.erase(copy.begin(), copy.begin()+n);
	return copy;
}

std::string Util::concat(const std::vector<std::string>& strings, char delim)
{
	std::string combined;
	for (const auto& str : strings)
		combined = combined + str + delim;
	return combined;
}
