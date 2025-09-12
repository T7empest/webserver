//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>


class TcpServer
{
public:
	TcpServer() = default;
	~TcpServer();

	bool run();

private:
	bool setup_listen_socket();

	uint32_t address_ = INADDR_ANY;
	uint32_t port_    = 8080;

	sockaddr_in addr_{};
	int         server_fd_ = 0;

	sockaddr* current_client_;
	int       client_fd_ = 0;
};
