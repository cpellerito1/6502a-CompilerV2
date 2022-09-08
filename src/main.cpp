#include "Compiler.h"
#include <fstream>


// Blueprints
std::string readFile(std::string);

int main(int argc, char *argv[]) {
	std::string inputString;
	// Make sure a command line argument was supplied
	if (argc >= 1) {
		//inputString = readFile(argv[1]);
		inputString = readFile("test.txt");

		// Start the compiler
		std::cout << "Starting compiler..." << std::endl;
		Compiler c;
		c.lexer(inputString);

	} else {
		std::cout << "Please provide an input file";
	}

	return 0;
}

std::string readFile(std::string file) {
	// Try to open the file
	std::ifstream inputFile(file);
	// Make sure the file is open and then read its contents into a string
	if (inputFile.is_open()) {
		std::string contents((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
		return contents;
	} else {
		std::cout << "Could not open input file, please try again.";
		exit(-1);
	}
}