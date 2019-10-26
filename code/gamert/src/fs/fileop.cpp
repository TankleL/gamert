#include "fileop.hpp"

using namespace std;
using namespace fileop;

void fileop::read_binary_file(std::vector<std::uint8_t>& dest, const std::string& path)
{
	try
	{
		ifstream istream(path, ios::binary);
		istream.seekg(0, ios::end);
		std::size_t len = istream.tellg();
		dest.resize(len);
		istream.seekg(0);
		istream.read((char*)&dest[0], len);
	}
	catch (std::exception & ex)
	{
		throw ex;
	}
}


