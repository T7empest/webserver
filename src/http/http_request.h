//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <string>
#include <sstream>

namespace Http
{
	struct Request
	{
		std::string method;
		std::string url;
		std::string version;

		std::vector<std::string> headers;
	};

	inline Request parse_request(const std::string& request_str)
	{
		std::istringstream stream(request_str);
		Request            req;
		stream >> req.method >> req.url >> req.version;
		// headers
		std::string line;
		std::getline(stream, line); // consume rest of the line after version
		while (std::getline(stream, line))
		{
			if (line.empty() || line == "\r")
				break;
			line.pop_back();
			req.headers.push_back(line);
		}
		return req;
	}

	inline std::string get_connection_status(const Request& request)
	{
		for (std::string header : request.headers)
		{
			if (header == "Connection: close")
				return "close";

			if (header == "Connection: keep-alive")
				return "keep-alive";
		}
		// not found
		return "";
	}
}
