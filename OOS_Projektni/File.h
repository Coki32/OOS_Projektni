#pragma once
#include "FileSystem.h"
class File
{
	size_t nodeID;
	bool open;

	friend class FileSystem;
};

