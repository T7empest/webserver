//
// Created by Lukáš Blažek on 26.09.2025.
//

#pragma once

#include <cstddef>
#include <string>

namespace Http
{
	inline constexpr size_t MAX_HEADER_COUNT  = 100;
	inline constexpr size_t MAX_HEADERS_BYTES = 16384; // 16 KB

	inline constexpr std::string CRLF     = "\r\n";
	inline constexpr std::string CRLFCRLF = "\r\n\r\n";

	enum class Status
	{
		OK = 200,
		NOT_FOUND = 404
	};
}
