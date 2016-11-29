#pragma once

#include <string>
#include <iostream>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define signalError(x) signalErrorFunction(x, __FILENAME__, __LINE__)
#define logError(x) logErrorFunction(x, __FILENAME__, __LINE__)
#define logVar(x) do { std::cout << #x << ": " << x << "\n"; } while(0)

inline void signalErrorFunction(std::string message, std::string filename, int line){
	std::cerr << filename << "[" << line << "] " << message << "\n";
	exit(-1);
}

inline void logErrorFunction(std::string message, std::string filename, int line){
	std::cerr << filename << "[" << line << "] " << message << "\n";
}