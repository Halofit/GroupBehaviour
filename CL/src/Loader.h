#pragma once

#include <string>
#include <fstream>

void loadWalls(std::string filename){
	char el;
	std::fstream f(filename, std::fstream::in);

	while (f >> std::noskipws >> el) {
		switch(el){
			case 'o':
				break;
			case '-':
				break;
			case 's':
				break;
			case '\n':
				//newline
				break;
		}
	}
}
