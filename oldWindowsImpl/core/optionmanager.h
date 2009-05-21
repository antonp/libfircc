#ifndef _OPTIONMANAGER2_H_
#define _OPTIONMANAGER2_H_

/* New optionmanager, using a hashtable to be flexible
Advantages:
	* Dynamic - must not know about optionnames in advance
	* Fast - finding the index of an option in the table has the time complexity O(1) in many cases
	* Can handle any number of options
Disadvantages:
	* Memory usage - Allocates some memory that might never be used
	* Can not change size at runtime (Although there might be a workaround for this)
*/

#include <list>
#include <fstream>
#include "option.h"

#define SETTINGS OptionManager::getInstance()

class OptionManager {
public:
	static OptionManager *getInstance();
	static void deleteInstance();

	// Normal public functions ;)
	bool loadFromFile();
	std::string getString(const std::string &var);
	int getInt(const std::string &var);

#ifdef _DEBUG
	void showTable();
#endif
private:
	OptionManager();
	~OptionManager();
	static OptionManager *instance;
	enum {
		TABLE_SIZE=10
	};

	std::list<Option> table[TABLE_SIZE];

	int getOptionIndex(const std::string &optionName);

};


#endif // _OPTIONMANAGER2_H_