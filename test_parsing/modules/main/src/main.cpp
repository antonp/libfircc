#include <pcrecpp.h>
#include <string>
#include <iostream>
#include <tokenizer.h>
/**
	Split the parameters into separate strings.

	Might be worth investigating how PCRE can help out with this
	or if it can be made faster with an alternative tokenizer
	which doesn't modify the source string but uses an offset
	instead.
*/
void parseParams(std::string &all, std::string list[])
{
	using std::cout;
	using std::endl;
	using std::string;
	using tokenizer::tokenize;
	// Assuming list holds 15 elements
	cout << endl << "all params: " << all << endl;

	for ( int i=0; i<15; i++ )
	{
		if ( all[0] == ':' )
		{
			list[i] = all.substr(1);
			break;
		} else if ( !tokenize(list[i], all, " ") )
		{
			break;
		}
	}
}

	void parseMessage(const std::string &message)
	{
		using pcrecpp::RE;

		std::string prefix, command, firstParam, parameters;
		std::string params[15];

		std::cout << "## parseMsg(): " << message << "##" << std::endl;

		RE pattern(
"^(:(\\S+?)\\s)?(\\S+)\\s?(:?.*)$"
		);

		std::cout << "FullMatch returned: " <<
			pattern.FullMatch(message, (void *)0, &prefix, &command,
			&parameters)

			<< ", prefix: " << prefix
			<< ", command: " << command;

		parseParams(parameters, params);

		for ( int i=0; i<15; i++ )
		{
			std::cout << ", params["<<i<<"] : '" << params[i] << "'\n";
		}
		std::cout << std::endl;
	}

int main(int argc, char *argv[])
{
	if ( argc > 1 )
	{
		parseMessage(argv[1]);
	}
	parseMessage(":bail join #mychan");
	parseMessage(":WiZ!jto@tolsun.oulu.fi PART #playzone :I lost");

	if ( argc > 1 )
	{
		using pcrecpp::RE;
		RE numPattern("^[[:digit:]]{3}$");
		std::cout << argv[1] << " numeric? "
			<< numPattern.FullMatch(argv[1])
			<< std::endl;
	}

	return 0;
}
