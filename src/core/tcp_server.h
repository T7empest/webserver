//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "thread_pool.h"

class TcpServer
{
public:
	TcpServer() : thread_pool_(std::jthread::hardware_concurrency())
	{
		std::cout << std::jthread::hardware_concurrency() << std::endl;
	}

	~TcpServer();

	int         run();
	static void handle_client(int client_fd);

private:
	bool setup_listen_socket();

	uint32_t address_ = INADDR_ANY;
	uint32_t port_    = 8080;

	int        server_fd_ = 0;
	ThreadPool thread_pool_;
};

// TODO: 	1.	Avoid double recv: read until \r\n\r\n, then respond; every syscall matters.
// TODO:	2.	Prebuild static response once (headers+body) and reuse the same std::string/buffer to avoid formatting per request.
// TODO:	3.  Handle keep-alive requests, not always Connection: close
//
