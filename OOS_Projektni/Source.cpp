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
	fs.mkdir("root/mace");
	fs.put("slika.jpg", "root/mace/slika.jpg");
	fs.put("t1.jpg", "root/mace/slika2.jpg");
	fs.put("bigger.jpg", "root/mace/slika3.jpg");
	fs.mkdir("root/tekst");
	fs.create("root/tekst/dzej.txt");
	fs.echo("root/tekst/dzej.txt", "Da l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\n");
	fs.cat("root/tekst/dzej.tt");
	fs.cat("root/tekst/dzej.txt");
	fs.mkdir("root/kopije");
	fs.cp("root/tekst/dzej.txt", "root/kopije/dzje-copy.txt");
	fs.cat("root/kopije/dzje-copy.txt");
	fs.get("root/kopije/dzje-copy.txt", "dalsiikada.txt");
	fs.get("root/mace/slika3.jpg", "velikaMaca.jpg");
	fs.ls(0, true);
	//fs.ls("root/slike5/", true);

}
void pisiProstor(const FileSystem& fs) {
	std::cout << "Dostupno prostora: " << fs.getFreeSpace() << "b " << std::endl;
}

int main() {
	f();
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