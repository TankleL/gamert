#pragma once
#include "pre-req.hpp"

namespace utils_string
{
	/**
	 * @func are_strings_equal
	 * @brief judge whether the given two strings are equal in
	 * a specific meaning.
	 */
	bool are_strings_equal(
		const std::string& str_short,
		const std::string& str_long,
		size_t viewbeg_long,
		char endch_long);
}
