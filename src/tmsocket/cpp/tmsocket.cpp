#include "tmsocket/include/tmsocket.hpp"
#include <prep/include/os_net.h>
#include <mutex>

TMSOCKET_NAMESPACE_BEGIN

#if defined(PREP_WINDOWS)

static ::std::mutex initialize_network_mtx;

void
tmsocket::initialize_network()
{
	::std::unique_lock<::std::mutex> lock(initialize_network_mtx);

	if (tmsocket::network_initialized())
	{
		return;
	}

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);

	auto err = WSAStartup(wVersionRequested, &wsaData);
	if (err)
	{
		throw initialize_network_failed("Fail to initialize WSA 2.2.", err);
	}
	tmsocket::initialized = true;
}

void
tmsocket::end_network()
{
	::std::unique_lock<::std::mutex> lock(initialize_network_mtx);

	if (!tmsocket::network_initialized())
	{
		return;
	}
	(void)::WSACleanup();
	tmsocket::initialized = false;
}

bool
tmsocket::initialized = false;

#else

void
tmsocket::initialize_network()
{
	/* Do nothing. */
}

void
tmsocket::end_network()
{
	/* Do nothing. */
}

bool
tmsocket::initialized = true;

#endif // defined(PREP_WINDOWS)


bool
tmsocket::network_initialized()
{
	return tmsocket::initialized;
}

TMSOCKET_NAMESPACE_END
