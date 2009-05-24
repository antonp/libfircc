#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <string>

namespace tokenizer {
	void tokenize(std::string &dest, std::string &source, char delimeter=' ');
	bool tokenize(std::string &dest, std::string &source, const char *delimeter);
};

#endif // _TOKENIZER_H_