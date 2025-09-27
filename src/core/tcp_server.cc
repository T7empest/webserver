//
// Created by Lukáš Blažek on 12.09.2025.
//

#include "tcp_server.h"

#include <iostream>
#include <thread>
#include <unistd.h>

#include "constants.h"
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
		thread_pool_.enqueue([this, client_fd]
		{
			handle_client(client_fd);
		});
	}
	return 0;
}

void TcpServer::handle_client(int client_fd)
{
	std::string request;
	request.reserve(16384);


	bool closed = false;
	while (!closed)
	{
		size_t n = recv_until_end(client_fd, request);
		if (n <= 0) break;
		Http::Request req = Http::parse_request(request);

		bool should_close = (req.version == "HTTP/1.0") ||
			(Http::get_connection_status(req) == "close");

		dev_send_response(client_fd, should_close, req);

		closed = should_close;
	}
	close(client_fd);
}

void TcpServer::dev_send_response(int client_fd, bool closed, Http::Request req)
{
	std::string              body{};
	std::vector<std::string> headers{};
	Http::Response           res{};
	std::string              res_str{};

	// TODO: change the endpoint configuration
	if (req.url == "/health")
	{
		body    = Http::test_body;
		headers = {Http::test_content_length};
		if (closed) headers.emplace_back("Connection: close");
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
		if (closed)
		{
			send(client_fd, Http::NOT_FOUND_RESPONSE_CLOSE.c_str(),
				 Http::NOT_FOUND_RESPONSE_CLOSE.size(), 0);
		}
		else
		{
			send(client_fd, Http::NOT_FOUND_RESPONSE_KEEPALIVE.c_str(),
				 Http::NOT_FOUND_RESPONSE_KEEPALIVE.size(), 0);
		}
	}
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

// reads headers only for now
size_t TcpServer::recv_until_end(int client_fd, std::string& buf)
{
	buf.clear();
	buf.reserve(Http::MAX_HEADERS_BYTES);
	// reading in 4KB chunks
	char tmp[4096];

	while (buf.size() < Http::MAX_HEADERS_BYTES)
	{
		size_t  to_read = std::min(sizeof(tmp), Http::MAX_HEADERS_BYTES - buf.size());
		ssize_t n       = recv(client_fd, tmp, to_read, 0);
		if (n <= 0) break; // 0 = client closed, <0 = error

		size_t old_size = buf.size();
		buf.append(tmp, static_cast<size_t>(n));

		// Look for CRLFCRLF marker across chunk boundaries
		size_t scan_from = (old_size >= 3) ? old_size - 3 : 0;
		size_t pos       = buf.find(Http::CRLFCRLF, scan_from);
		if (pos != std::string::npos)
		{
			buf.resize(pos + 4); // keep headers only
			break;
		}
	}
	return buf.size();
}
