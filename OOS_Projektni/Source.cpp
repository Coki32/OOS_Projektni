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
		auto folderID = fs.mkdir("root/goddamnit");
		 folderID = fs.mkdir("root/slike5");
		 folderID = fs.mkdir("root/slike5/podslike");
		 fs.mkdir("root/goddamnit/boi");
		 fs.mkdir("root/slike5/drugeSlike");
		 fs.mkdir("root/slike5/drugeSlike/marko");
		 fs.mkdir("root/slike5/drugeSlike/marko/darko");
		 fs.mkdir("root/slike5/ivan");
		 fs.mkdir("root/slike2/car");
		auto slikaID = fs.writeFile(data, "root/maca23.jpg",true);
		slikaID = fs.writeFile(data, "root/slike5/drugeSlike/marko/darko/maca23.jpg",true);
		slikaID = fs.cp("root/maca23.jpg", "root/marko/podslike/kopijaMace2.jpg");
		fs.cp("root/maca23.jpg", "root/slike5/podslike/I JOS KOPIJA.jpg");
		auto readSlika = fs.readFile(slikaID);
		std::fstream outSlika = std::fstream("t1-copy.jpg", std::ios::binary | std::ios::out);
		fs.create("root/marko.png");
		fs.create("root/ivanIvanovic.jpg");
		fs.create("root/slike5/ivan/ivanProfilna.jpg");
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
	fs.ls("", true);
	//fs.ls("root/slike5/", true);

}
void pisiProstor(const FileSystem& fs) {
	std::cout << "Dostupno prostora: " << fs.getFreeSpace() << "b " << std::endl;
}
void createAndEcho() {
	//f();
	FileSystem fs(DEFAULT_FILENAME);
	fs.mkdir("root/folderA");
	fs.create("root/folderA/fajla.a");
	fs.create("root/folderA/fajlb.a");
	fs.create("root/folderA/fajlc.a");
	fs.echo("root/folderA/fajlc.a", "Neki tekst u fajlu c");
	fs.echo("root/folderA/fajlb.a", "Da li echo prepisuje fajl? Trebalo bi da prepisuje...");
	fs.echo("root/folderA/fajla.a", "U teoriji nema maks duzine fajla duuuu zeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");

	fs.cat("root/folderA/fajla.a");
	fs.cat("root/folderA/fajlb.a");
	fs.cat("root/folderA/fajlc.a");

	fs.mkdir("root/folderB");
	fs.mkdir("root/folderE");
	fs.mkdir("root/folderBFAWDWA");
	fs.ls(0, true);
	fs.rename("root/folderA", "nijeViseFolderA");
	fs.ls(0, true);
}

int main() {
	//f();
	createAndEcho();
	//testFind();
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