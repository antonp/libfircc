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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

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
