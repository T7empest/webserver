//
// Created by Lukáš Blažek on 13.09.2025.
//

#pragma once

//
// Created by Lukáš Blažek on 12.09.2025.
//

#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace Http
{
	enum class Status
	{
		OK,
		NOT_FOUND
	};

	inline std::string status_to_string(Status status)
	{
		switch (status)
		{
		case Status::OK:
			return "HTTP/1.1 200 OK";
		case Status::NOT_FOUND:
			return "HTTP/1.1 404 Not Found";
		default:
			return "HTTP/1.1 500 Internal Server Error";
		}
	}

	struct Response
	{
		Status status;
		std::vector<std::string> headers;
		std::string body;

		std::string to_string() const
		{
			std::ostringstream response_stream;
			response_stream << status_to_string(status) << "\r\n";
			for (const auto& header : headers)
			{
				response_stream << header << "\r\n";
			}
			response_stream << "\r\n";
			response_stream << body;
			return response_stream.str();
		}
	};
}
