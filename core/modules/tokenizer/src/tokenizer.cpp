/*

Copyright (c) 2010, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

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
