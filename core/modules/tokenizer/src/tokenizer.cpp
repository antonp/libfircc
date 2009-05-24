#include "../tokenizer.h"
#include <string.h>

void tokenizer::tokenize(std::string &dest, std::string &source, char delimeter) {
	size_t pos = source.find(delimeter);
	if ( pos != std::string::npos ) {
		dest = source.substr(0, pos);
		source.erase(0, pos+1);
	} else {
		dest = source;
		source.erase();	// Hopefully clears source completely
	}
	//	std::cout << "[Tokenizer] Dest: " << dest << " Source: " << source << std::endl;
}

bool tokenizer::tokenize(std::string &dest, std::string &source, const char *delimeter) {
	size_t pos = source.find(delimeter);
	if ( pos != std::string::npos ) {
		dest = source.substr(0, pos);
		source.erase(0, pos+strlen(delimeter));
		return true;
	} else {
		dest = source;
		source.erase();
		return false;
	}
}
