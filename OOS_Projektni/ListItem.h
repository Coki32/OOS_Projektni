#pragma once
#include <string>
#include <memory>

struct ListItem
{
	std::string name;
	int nodeIndex;

	ListItem(const std::string& str, int idx) : name(str), nodeIndex(idx) {}
};

