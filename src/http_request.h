//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <string>
#include <sstream>

namespace Http
{
	const std::string OK        = "HTTP/1.1 200 OK\r\n\r\n";
	const std::string NOT_FOUND = "HTTP/1.1 404 Not Found\r\n\r\n";

	struct Request
	{
		std::string method;
		std::string url;
		std::string version;
	};

	inline Request parse_request(const std::string& request_str)
	{
		std::istringstream stream(request_str);
		Request            req;
		stream >> req.method >> req.url >> req.version;
		return req;
	}
}
