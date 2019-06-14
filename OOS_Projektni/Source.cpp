#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "FileSystem.h"

#include "Util.h"
int main() {
	std::unique_ptr<FileSystem> fs;
	try {
		fs = std::make_unique<FileSystem>("sistem2.ext4nt");
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return 0;
	}
	std::string line;
	do {
		std::cout << "command: ";
		std::getline(std::cin, line);
		auto split = Util::stringSplit(line, ' ');
		auto command = split[0];
		auto rest = Util::stringsAfter(split, 1);
		if (command == "mkdir")
		{
			if (rest.size() != 1) {
				std::cout << "mkdir prima 1 argument i to je putanja foldera pocevsi od root/" << std::endl;
			}
			else if (rest[0]._Starts_with("root/")) {
				auto res = fs->mkdir(rest[0]);
				if (res != -1) {
					std::cout << "Kreiran folder " << rest[0] << std::endl;
				}
			}
			else
				std::cout << "Naziv foldera mora pocinjati sa root/..." << std::endl;
		}
		else if (command == "ls") {
			if (rest.size() == 0)
				fs->ls("root", false);
			else {
				bool recursive = false, full = false;
				std::string path = "root/";
				for (const auto& option : rest)
					if (option == "-r")
						recursive = true;
					else if (option._Starts_with("root/"))
						path = option;
					else if (option == "-f")
						full = true;
					else if (option == "--help")
						std::cout << "Dostupne opcije su:" << std::endl <<
						"-r za rekurzivni ispis svih podfoldera i njihovih fajlova" << std::endl <<
						"-f za prisilno ispisivanje i root elementa (inace izostavljen), \"override\" path argument ako je proslijedjen" << std::endl;
				if (full)
					fs->ls(0, recursive);
				else
					fs->ls(path, recursive);
			}
		}
		else if (command == "create") {
			if (rest.size() != 1) {
				std::cout << "create prima jedan argument i to naziv fajla (puna putanja)" << std::endl;
			}
			else if (rest[0]._Starts_with("root/")) {
				auto ret = fs->create(rest[0]);
				if (ret != -1)
					std::cout << "Uspjesno kreiran fajl " << rest[0] << std::endl;

			}
			else {
				std::cout << "Putanja fajla mora pocinjati sa root/..." << std::endl;
			}
		}
		else if (command == "put") {
			if (rest.size() != 2) {
				std::cout << "put ocekuje 2 argumenta: " << std::endl << "prvi argument putanja fajla koji kopiras na sistem" << std::endl <<
					"drugi argument putanja gdje na sistem da ga sacuva" << std::endl;
			}
			else if (!rest[1]._Starts_with("root/"))
				std::cout << "Drugi argument mora pocinjati sa apsolutnom putanjom do fajla (root/...)" << std::endl;
			else {
				auto ret = fs->put(rest[0], rest[1]);
				if (ret != -1)
					std::cout << "Fajl uspjesno kreiran!" << std::endl;
			}
		}
		else if (command == "get") {
			if (rest.size() != 2) {
				std::cout << "get ocekuje 2 argumenta: " << std::endl << "prvi argument putanja fajla na sistemu koji \"skidas\"" << std::endl <<
					"drugi argument putanja gdje na (host) sistem da ga sacuva" << std::endl;
			}
			else if (!rest[0]._Starts_with("root/"))
				std::cout << "Drugi argument mora pocinjati sa apsolutnom putanjom do fajla (root/...)" << std::endl;
			else {
				if (fs->get(rest[0], rest[1]))
					std::cout << "Fajl uspjesno \"skinut\" i upisan u " << rest[1] << std::endl;
			}
		}
		else if (command == "cp") {
			if (rest.size() != 2) {
				std::cout << "cp ocekuje 2 argumenta: " << std::endl << "prvi argument putanja fajla koji kopiras (apsolutna, root/...)" << std::endl <<
					"drugi argument putanja kopije" << std::endl;
			}
			else if (!rest[1]._Starts_with("root/") && !rest[0]._Starts_with("root/"))
				std::cout << "Oba arugmenta moraju pocinjati sa apsolutnom putanjom do fajla (root/...)" << std::endl;
			else {
				auto ret = fs->cp(rest[0], rest[1]);
				if (ret != -1)
					std::cout << "Fajl uspjesno kopiran!" << std::endl;
			}
		}
		else if (command == "mv") {
			if (rest.size() != 2) {
				std::cout << "mv ocekuje 2 argumenta: " << std::endl << "prvi argument putanja fajla koji premjesta (apsolutna, root/...)" << std::endl <<
					"drugi arument je putanja do foldera gdje treba smjestiti dati fajl (isto ime zadrzava)" << std::endl;
			}
			else if (!rest[1]._Starts_with("root/") && !rest[0]._Starts_with("root/"))
				std::cout << "Oba arugmenta moraju pocinjati sa apsolutnom putanjom do fajla (root/...)" << std::endl;
			else {
				auto ret = fs->mv(rest[0], rest[1]);
				if (ret)
					std::cout << "Fajl uspjesno premjesten!" << std::endl;
			}
		}
		else if (command == "rename") {
			if (rest.size() != 2) {
				std::cout << "rename ocekuje 2 argumenta: " << std::endl << "prvi argument putanja fajla/foldera koji treba da se preimenuje (apsolutna, root/...)" << std::endl <<
					"drugi arument je novi naziv datog fajla/foldera" << std::endl;
			}
			else if (!rest[0]._Starts_with("root/"))
				std::cout << "Prvi argument mora biti apsolutna putanja (root/...)" << std::endl;
			else {
				auto ret = fs->rename(rest[0], rest[1]);
				if (ret)
					std::cout << "Fajl uspjesno preimenovan!" << std::endl;
			}
		}
		else if (command == "echo") {
			if (rest.size() == 0)
				std::cout << "echo prima 2 argumenta:" << std::endl <<
				"prvi putanja do fajla u koji se upisuje (prepisuje, ne append)" << std::endl <<
				"drugi je novi sadrzaj fajla" << std::endl;
			else if (!rest[0]._Starts_with("root/")) {
				std::cout << "Prvi argument mora biti apsolutna putanja do fajla u koji upisujes!" << std::endl;
			}
			else if (rest.size() > 1) {
				fs->echo(rest[0], Util::concat(Util::stringsAfter(rest, 1), ' '));
			}
			else
				std::cout << "Nakon putanje mora doci tekst koji se upisuje u fajl!" << std::endl;
		}
		else if (command == "cat") {
			if (rest.size() == 0)
				std::cout << "cat prima jedan argument i to je apsolutna putanja do fajla koji se ispisuje" << std::endl;
			else if (!rest[0]._Starts_with("root/"))
				std::cout << "Prvi argument za cat mora biti apsolutna putanja do fajla!" << std::endl;
			else
				fs->cat(rest[0]);
		}
		else if (command == "rm") {
			if (rest.size() == 0) {
				std::cout << "rm prima jedan argument, a to je fajl koji brise" << std::endl <<
					"Opciono, uz -r flag brise citav folder sa svim sadrzajem" << std::endl;
			}
			else if (rest.size() == 1) {
				if (rest[0]._Starts_with("root/"))
					fs->rm(rest[0]);
				else
					std::cout << "Prvi argument mora biti apsolutna putanja fajla koji brise!" << std::endl;
			}
			else if (rest.size() == 2) {
				bool recursive = false;
				std::string path = "NOT_SET";
				for (const auto& opt : rest)
					if (opt == "-r")
						recursive = true;
					else if (opt._Starts_with("root/"))
						path = opt;
					else
						std::cout << "Nepoznata opcija " << opt << std::endl;
				if (path == "NOT_SET")
					std::cout << "Putanja je obavezna za rm!" << std::endl;
				else
					fs->rm(path, recursive);
			}

		}
		else if (command == "stat") {
			if (rest.size() == 0)
				fs->info();
			else if (rest.size() == 1 && rest[0] == "--help") {
				std::cout << "stat ispisuje informacije o cvoru i blokovima datog fajla (ili foldera)" << std::endl <<
					"Ako se navede nijedan argument onda ispisuje informacije o samom sistemu (broj dostupnih cvorova, blokova, prostor itd)" << std::endl;
			}
			else if (rest.size() >= 1 && rest[0]._Starts_with("root/"))
				fs->stat(rest[0]);
		}
	} while (line.compare("quit") != 0);
	return 0;
}