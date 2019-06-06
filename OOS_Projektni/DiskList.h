#pragma once
#include <memory>
#include <vector>

#include "Data.h"
#include "ListItem.h"

struct DiskList
{

	static std::shared_ptr<Data> toData(const std::vector<ListItem>& items);

	static std::vector<ListItem> fromData(const std::shared_ptr<Data>& data);
};

