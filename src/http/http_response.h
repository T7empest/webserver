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

#include "constants.h"

namespace Http
{
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
		Status                   status;
		std::vector<std::string> headers;
		std::string              body;

		[[nodiscard]] std::string to_string() const
		{
			std::ostringstream response_stream;
			response_stream << status_to_string(status) << CRLF;
			for (const auto& header : headers)
			{
				response_stream << header << CRLF;
			}
			response_stream << CRLF;
			response_stream << body;
			return response_stream.str();
		}
	};

	inline const std::string NOT_FOUND_RESPONSE_CLOSE =
		"HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";

	inline const std::string NOT_FOUND_RESPONSE_KEEPALIVE =
		"HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n";

	// for test response
	inline constexpr std::string test_body           = "test";
	inline constexpr std::string test_content_length = "Content-Length: 4";
}
