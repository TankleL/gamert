#include "utils-string.hpp"

using namespace utils_string;

bool utils_string::are_strings_equal(
	const std::string& str_short,
	const std::string& str_long,
	size_t viewbeg_long,
	char endch_long)
{
	if (str_long.length() - viewbeg_long >= str_short.length())
	{
		size_t cmp_length = str_short.length();
		for (auto i = 0; i < cmp_length; ++i)
		{
			if (str_short[i] != str_long[i + viewbeg_long])
				return false;
		}

		if (str_long.length() - viewbeg_long > cmp_length)
		{
			if (str_long[cmp_length + viewbeg_long] == endch_long)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
