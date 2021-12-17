#ifndef TMSOCKET_TMSOCKET_HPP__
#define TMSOCKET_TMSOCKET_HPP__

#include <tmsocket/include/defs.hpp>
#include <tmsocket/include/netexcept.hpp>

TMSOCKET_NAMESPACE_BEGIN

class tmsocket
{
public:
	static void initialize_network();
	static void end_network();
	static bool network_initialized();

private:
	static bool initialized;
};

TMSOCKET_NAMESPACE_END

#endif // !TMSOCKET_TMSOCKET_HPP__
