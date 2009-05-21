#ifndef _OPTION_H_
#define _OPTION_H_

#include <string>

class Option {
public:
	Option(std::string n, std::string v=""):
	  name(n), value(v) {}
	
	std::string &getName() {
		return name;
	}
	std::string value;
private:
	// name is read-only and value is read-write
	std::string name;
};

#endif // _OPTION_H_