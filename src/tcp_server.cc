//
// Created by Lukáš Blažek on 12.09.2025.
//

#include "tcp_server.h"

#include <iostream>
#include <unistd.h>

#include "http_request.h"

TcpServer::~TcpServer()
{
}

bool TcpServer::run()
{
	if (!setup_listen_socket()) return false;

	for (int i = 0; i < 2; ++i)
	{
		std::cout << "server running: waiting for connection" << std::endl;

		socklen_t clen = sizeof(sockaddr_in);
		client_fd_     = accept(server_fd_, current_client_, &clen);
		if (client_fd_ < 0)
		{
			std::cerr << "accept failed\n";
			return false;
		}

		std::string request{};
		request.resize(16384);

		const ssize_t req_size = recv(client_fd_, request.data(), request.size(), MSG_PEEK);
		request.resize(req_size);

		if (recv(client_fd_, request.data(), request.size(), 0) == 0)
		{
			std::cerr << "wrong request data";
			return false;
		}

		Http::Request req = Http::parse_request(request);
		if (req.url == "/health")
		{
			send(client_fd_, Http::OK.c_str(), Http::OK.size(), 0);
		}
		else
		{
			send(client_fd_, Http::NOT_FOUND.c_str(), Http::NOT_FOUND.size(), 0);
		}

		close(client_fd_);
	}
	return true;
}

bool TcpServer::setup_listen_socket()
{
	server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd_ < 0)
	{
		std::cerr << "socket failed\n";
		return false;
	}

	constexpr int reuse = 1; // true, to reuse the port
	setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// the address: any ip, port 8080
	sockaddr_in addr{};
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(address_);
	addr.sin_port        = htons(port_);

	if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
	{
		std::cerr << "bind failed\n";
		return false;
	}

	if (listen(server_fd_, 5) != 0)
	{
		std::cerr << "listen failed\n";
		return false;
	}

	return true;
}
