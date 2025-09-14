//
// Created by Lukáš Blažek on 12.09.2025.
//

#include "tcp_server.h"

#include <iostream>
#include <thread>
#include <unistd.h>

#include "../http/http_request.h"
#include "../http/http_response.h"

TcpServer::~TcpServer()
{
}

int TcpServer::run()
{
	if (!setup_listen_socket()) return false;

	std::cout << "server running: waiting for connections" << std::endl;

	while (true)
	{
		sockaddr_in cli{};
		socklen_t   clen = sizeof(sockaddr_in);

		int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&cli), &clen);

		if (client_fd < 0)
		{
			std::cerr << "accept failed\n";
			return -1;
		}
		thread_pool_.enqueue([this, client_fd]{
			handle_client(client_fd);
		});
	}
	return 0;
}

void TcpServer::handle_client(int client_fd)
{
	std::string request;
	request.reserve(16384);
	char buf[4096];

	ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
	if (n <= 0)
	{
		close(client_fd);
		return;
	}
	request.append(buf, n);

	Http::Request            req = Http::parse_request(request);
	std::string              body{};
	std::vector<std::string> headers{};
	Http::Response           res{};
	std::string              res_str{};
	if (req.url == "/health")
	{
		body    = "123 test 123";
		headers = {
			std::format("Content-Length: {}", body.size()),
			std::string("Connection: close")
		};
		res = {
			.status = Http::Status::OK,
			.headers = headers,
			.body = body
		};
		res_str = res.to_string();
		send(client_fd, res_str.c_str(), res_str.size() * sizeof(char), 0);
	}
	else
	{
		body    = "";
		headers = {
			std::format("Content-Length: {}", body.size()),
			std::string("Connection: close")
		};
		res = {
			.status = Http::Status::NOT_FOUND,
			.headers = headers,
			.body = body
		};
		res_str = res.to_string();
		send(client_fd, res_str.c_str(), res_str.size() * sizeof(char), 0);
	}

	close(client_fd);
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

	if (listen(server_fd_, SOMAXCONN) != 0)
	{
		std::cerr << "listen failed\n";
		return false;
	}

	return true;
}
