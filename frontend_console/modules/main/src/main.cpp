#include <iostream>

#include <firc.h>

int main(int argc, char *argv[])
{
	std::cout << "Hello, world!" << std::endl;
	
	firc::Core fircCore;
	
	fircCore.init();
	
	while ( true == fircCore.update() )
	{
		// Handle events from firc (fircCore.update(&event))
	}
	
	return 0;
}
