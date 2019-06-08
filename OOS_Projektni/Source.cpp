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
	std::fstream slika = std::fstream("bigger.jpg", std::ios::binary | std::ios::in);
	if (slika.is_open()) {
		slika.seekg(0, std::fstream::end);
		auto len = slika.tellg();
		char* ar = new char[len];
		slika.seekg(0);
		slika.read(ar, len);
		auto data = std::make_shared<Data>(ar, len);
		auto folderID = fs.mkdir("root/slike");
		auto slikaID = fs.writeFile(data, "root/slike/maca.jpg");
		auto readSlika = fs.readFile(slikaID);
		std::fstream outSlika = std::fstream("t1-copy.jpg", std::ios::binary | std::ios::out);
		if (outSlika.is_open()) {
			outSlika.write(readSlika->data.get(), readSlika->length);
			outSlika.flush();
			outSlika.close();
		}
		else
			std::cout << "Ne mogu otvoriti izlaznu sliku!";
	}
	else
		std::cout << "Ne mogu otviriti ulaznu sliku!";

}

int main() {
	g();
	return 0;
}
int main2() {
	f();
	g();
	Bitmap bmp(256,1);
	bmp.setBit(0);
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