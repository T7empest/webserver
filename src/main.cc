#include <iostream>

#include "http_request.h"
#include "tcp_server.h"

int main()
{
	std::cerr << std::unitbuf; // logs -> stderr only

	TcpServer server{};
	bool success = server.run();

	if (!success) return -1;
	return 0;
}
