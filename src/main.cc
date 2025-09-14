#include <iostream>

#include "http/http_request.h"
#include "core/tcp_server.h"

int main()
{
	std::cerr << std::unitbuf; // logs -> stderr only

	TcpServer server{};
	bool success = server.run();

	if (!success) return -1;
	return 0;
}
