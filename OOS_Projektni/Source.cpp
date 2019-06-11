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
	fs.mkdir("root/tekst");
	fs.mkdir("root/kopija");
	fs.ls(0, true);
	fs.put("slika.jpg", "root/mace/slika.jpg");
	fs.put("t1.jpg", "root/mace/slika2.jpg");

	fs.put("bigAssSlika.jpg", "root/mace/bigAssSlika.jpg");
	fs.put("bigAssSlika.jpg", "root/mace/bigAssSlika12.jpg");

	fs.stat("root/mace/velikiZip.7z");
	fs.mkdir("root/tekst");
	fs.create("root/tekst/dzej.txt");
	fs.echo("root/tekst/dzej.txt", "Da l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\nDa l' si ikada\nMene voljela kao tebe ja\n");
	fs.cat("root/tekst/dzej.tt");
	fs.cat("root/tekst/dzej.txt");
	fs.mkdir("root/kopije");
	fs.cp("root/tekst/dzej.txt", "root/kopije/dzje-copy.txt");
	fs.cat("root/kopije/dzje-copy.txt");
	fs.get("root/kopije/dzje-copy.txt", "dalsiikada.txt");
	fs.get("root/mace/bigAssSlika.jpg", "nemaViseLimita.jpg");
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