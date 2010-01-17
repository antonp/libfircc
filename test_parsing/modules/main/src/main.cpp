#include <pcrecpp.h>
#include <string>
#include <iostream>

	void parseMessage(const std::string &message)
	{
		using pcrecpp::RE;

		std::string prefix, command, firstParam, parameters;

		//RE pattern("^(:(\\S+?))?\\s(\\S+?)\\s:?(\\S*)\\s?:?(.*)$");
		RE pattern("^(:(\\S+?))?\\s(\\S+?)\\s:?((\\S*)\\s?:?.*)$");
		// There is a prefix
		//RE pattern("^:(\\S+?)\\s(\\S+?)\\s.+");

		std::cout << "FullMatch returned: " <<
			pattern.FullMatch(message, (void *)0, &prefix, &command,
								//&firstParam,
								&parameters,
								&firstParam)
			<< ", prefix: " << prefix
			<< ", command: " << command
			<< ", firstParam: " << firstParam
			<< ", parameters: " << parameters
			<< std::endl;

	}

int main(int argc, char *argv[])
{
	if ( argc > 1 )
	{
		parseMessage(argv[1]);
	}
	parseMessage(":bail join #mychan");
	parseMessage(":WiZ!jto@tolsun.oulu.fi PART #playzone :I lost");

	return 0;
}
