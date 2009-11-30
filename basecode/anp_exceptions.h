#ifndef _ANP_EXCEPTIONS_H_
#define _ANP_EXCEPTIONS_H_

#include <stdexcept>

namespace anp
{
	class ConstructionException: public std::runtime_error
	{
	public:
		ConstructionException(const std::string &message):
		std::runtime_error(message) { }
	};
}
	
#endif // _ANP_EXCEPTIONS_H_
