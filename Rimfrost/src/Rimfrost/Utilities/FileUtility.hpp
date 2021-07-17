#pragma once

#include <fstream>

#include "Rimfrost\Utilities\Logger.hpp"
namespace Rimfrost
{
	inline void writefileBin(char* src, size_t numElemts, size_t elementSize, const std::string& path)
	{
		std::ofstream output(path, std::ios::binary);
		output.write(src, numElemts * elementSize);
		output.close();
		Logger::getLogger().debugLog("write " + std::to_string(numElemts * elementSize) + "bytes to " + path + "\n");
	}

	//pass zero in fileSize, to get the size returned as output, then call again with fileSize
	inline void readfileBin(char* dst, size_t& fileSize, const std::string& path)
	{
		std::ifstream input("outFile.txt", std::ios::binary);
		if (fileSize)
		{
			input.read(dst, fileSize);
		}
		else
		{
			input.seekg(0, input.end);
			fileSize = input.tellg();
		}
		input.close();
	}
}