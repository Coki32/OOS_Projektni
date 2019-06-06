#pragma once
#include <memory>
#include <vector>

#include "Data.h"
#include "ListItem.h"

struct DiskList
{

	static std::unique_ptr<Data> toData(const std::vector<ListItem>& items);

	static std::vector<ListItem> fromData(const std::unique_ptr<Data>& data);
};

