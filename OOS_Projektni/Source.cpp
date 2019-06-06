#include <iostream>
#include "INode.h"

#include <memory>
#include <vector>

#include "Bitmap.h"
#include "FileSystem.h"
#include "ListItem.h"
#include "DiskList.h"

void f() {
	FileSystem fs;
}

void g() {
	FileSystem fs(DEFAULT_FILENAME);
}
int main() {
	Bitmap bmp(256,1);
	bmp.setBit(14);
	bmp.setBit(15);
	bmp.setBit(16);
	bmp.setBit(3);
	bmp.setBit(9);
	bmp.setBit(20);
	bmp.setBit(29);
	bmp.setBit(40);
	auto e1 = bmp.findExtentStart_v(5);
	auto e2 = bmp.findExtentStart_v(15);
	auto e3 = bmp.findExtentStart_v(6);
	auto e4 = bmp.findExtentStart_v(9);
	auto e5 = bmp.findExtentStart_v(16);
	return 0;
}