//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "http_request.h"
#include "thread_pool.h"

class TcpServer
{
public:
	TcpServer() : thread_pool_(std::jthread::hardware_concurrency())
	{
		std::cout << "Thread count: " << std::jthread::hardware_concurrency() << std::endl;
	}

	~TcpServer();

	int         run();
	static void handle_client(int client_fd);
	static void dev_send_response(int client_fd, bool closed, Http::Request req);

private:
	bool   setup_listen_socket();
	static size_t recv_until_end(int client_fd, std::string& buf);

	uint32_t address_ = INADDR_ANY;
	uint32_t port_    = 8080;

	int        server_fd_ = 0;
	ThreadPool thread_pool_;
};

// TODO:	2.	Prebuild static response once (headers+body) and reuse the same std::string/buffer to avoid formatting per request.
// TODO:	NEW Handle keep-alive requests, not always Connection: close
//
