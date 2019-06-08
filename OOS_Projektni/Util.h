#pragma once

#include <vector>
#include <string>

#include "ListItem.h"


struct Util {

	static ListItem findByName(const std::vector<ListItem>& fileList, const std::string& name);

	static bool stringContains(const std::string& string, char c);

	static std::vector<std::string> stringSplit(const std::string& str, char delimiter);
};