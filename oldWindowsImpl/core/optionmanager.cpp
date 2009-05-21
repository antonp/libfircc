#include "optionmanager.h"

OptionManager::OptionManager() {

}

OptionManager::~OptionManager() {

}

OptionManager *OptionManager::getInstance() {
	if ( instance == NULL )
		instance = new OptionManager();
	return instance;
}

void OptionManager::deleteInstance() {
	if ( instance != NULL )
		delete instance;
	instance = NULL;
}

OptionManager *OptionManager::instance = NULL;

// Normal functions
bool OptionManager::loadFromFile() {
	// Expected format in file: optionName "new value"// Comment
//	INFOMSG("Loading config...");
	int linesOfCrap=1;

	// 1. Set the default settings (must be hardcoded)
/*	optionValues[BOTNICK]			= "cracker1";
	optionValues[SERVER_ADDRESS]	= "se.quakenet.org";
	optionValues[SERVER_PORT]		= "6668";
	optionValues[USER_FILE]			= "users.ini";
	optionValues[CHANNEL_FILE]		= "channels.ini";
	optionValues[PLUGIN_FILE]		= "plugins.ini";
	optionValues[VERSION_DISPLAY_MODE] = "2";
*/
	// 2. Open the file for reading
	std::ifstream fin("config.cfg");
	if ( !fin.is_open() ) {
//		con::adjustCursor(linesOfCrap);
//		_ERROR("FAILED");
//		WARNING("Failed to load config.cfg, falling back to default settings.");
		return false;
	}
	// 3. Read and parse it. Allow single-line-comments ( // )
	std::string line;
	while ( !fin.eof() ) {
		getline(fin, line);
		
		for ( unsigned int i=0; i<line.length(); i++ ) {
			if ( line[i] == '/' && i+1 <= line.length() && line[i+1] == '/' )
				line.erase(i, line.length()-i);
		}
//		std::cout << "[Config Parser] Line: \'" << line << '\'' << std::endl;
		
		// Ok we now have the actual "command" without comments and stuff
		// We need to check if it's a valid option now
		size_t spaceIndex = line.find(' ');
		if ( spaceIndex != std::string::npos ) {
			std::string field = line.substr(0, spaceIndex);
			size_t optionID = getOptionIndex(field);
			if ( optionID != -1 ) {
				size_t qStart = line.find('"', spaceIndex+1);
				size_t qEnd = line.find('"', qStart+1);
				if ( qStart != std::string::npos && qEnd != std::string::npos && qEnd-qStart >= 1 ) {
					std::string fieldValue = line.substr(qStart+1, qEnd-qStart-1);
					//optionValues[optionID] = fieldValue;
					table[optionID].push_back(Option(field, fieldValue));
				//	std::cout << "[Config Parser] Set " << options[optionID] << " to \'" << fieldValue << '\'' << std::endl;
				}// else
				//	std::cout << "[Config Parser] Line \'" << line << "\' isn't formatted correctly." << std::endl;
			} else {
//				std::cout << console::fg_yellow << "[Config Parser] WARNING: '" << field << "' isn't a valid option name." << console::fg_grey << std::endl;
//				WARNING('\'' << field << "' isn't a valid option name.");
				linesOfCrap++;
			}
		}// else
		//	std::cout << "[Config Parser] Line \'" << line << "\' isn't formatted correctly." << std::endl;
	}
	fin.close();
//	con::adjustCursor(linesOfCrap);
//	GREEN("OK");
	return true;
}

int OptionManager::getOptionIndex(const std::string &optionName) {
	// This is the hash function, modify it if needed
	
	return (optionName[0]*optionName[1]+optionName[2])%TABLE_SIZE;
}

std::string OptionManager::getString(const std::string &var) {
	std::list<Option>::iterator i;
	int index=getOptionIndex(var);

	// Loop through the linked list found at 'index' in the table, searching for 'var'
	for ( i=table[index].begin(); i!=table[index].end(); i++ ) {
		if ( (*i).getName() == var )
			return (*i).value;
	}

	return ""; // Really, really bad solution. Use a "dest" parameter passed by reference or C++ exceptions [TODO!!!]
}

int OptionManager::getInt(const std::string &var) {
	return atoi(getString(var).c_str());
}

#ifdef _DEBUG
void OptionManager::showTable() {
	std::cout << console::fg_white << "Showing options table..." << console::fg_grey << std::endl;

	for ( int i=0; i<TABLE_SIZE; i++ ) {
		std::cout << console::fg_yellow << i << ": " << console::fg_grey;
		std::list<Option>::iterator it;
		for ( it=table[i].begin(); it!=table[i].end(); it++ ) {
			std::cout << console::fg_green << 'X' << console::fg_grey;
		}
		BR();
	}
}
#endif
