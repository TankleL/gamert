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

std::vector<float> utils_string::strings_to_floats(
	const std::string& str,
	char separator)
{
	std::vector<float> res;

	const int max_tmp_len = 128;

	char tmp[max_tmp_len + 1] = "";
	int tmp_idx = 0;
	
	for (auto i = 0; i < str.length(); ++i)
	{
		if (str[i] != separator && tmp_idx < max_tmp_len)
		{
			tmp[tmp_idx] = str[i];
			++tmp_idx;
		}
		else if(str[i] == separator)
		{
			tmp[tmp_idx] = '\0';
			res.push_back((float)atof(tmp));
			tmp_idx = 0;
		}
		else
		{ // tmp_idx >= max_tmp_len
			throw std::runtime_error(
				"digits of the number are too long.");
		}
	}

	if (tmp_idx > 0)
	{
		tmp[tmp_idx] = '\0';
		res.push_back((float)atof(tmp));
	}

	return res;
}
