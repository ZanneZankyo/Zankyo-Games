#include "Persistence.hpp"
#include <stdio.h>

bool Persistence::LoadBinaryFileToBuffer(const std::string & filePath, std::vector<byte_t>& out_buffer)
{
	errno_t err;
	FILE *loadStream = nullptr;

	err = fopen_s(&loadStream, filePath.c_str(), "rb");
	if (err)
		return false;
	fseek(loadStream, 0, SEEK_END);
	size_t size = ftell(loadStream);
	fseek(loadStream, 0, SEEK_SET);
	out_buffer.resize(size);
	fread(out_buffer.data(), sizeof(byte_t), size, loadStream);
	if (loadStream)
	{
		err = fclose(loadStream);
		if (err)
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Persistence::SaveBufferToBinaryFile(const std::string & filePath, const std::vector<byte_t>& buffer)
{
	errno_t err;
	FILE *saveStream = nullptr;
	err = fopen_s(&saveStream, filePath.c_str(), "wb");
	if (err)
		return false;
	fwrite(buffer.data(), sizeof(byte_t), buffer.size(), saveStream);
	if (saveStream)
	{
		err = fclose(saveStream);
		if (err)
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Persistence::SaveStringToFile(const std::string& filePath, const std::string& string)
{
	errno_t err;
	FILE *saveStream = nullptr;
	err = fopen_s(&saveStream, filePath.c_str(), "wb");
	if (err)
		return false;
	fwrite(string.data(), sizeof(char), string.size(), saveStream);
	if (saveStream)
	{
		err = fclose(saveStream);
		if (err)
			return false;
		else
			return true;
	}
	else
		return false;
}

std::string Persistence::ReadBufferAsString(const std::vector<byte_t>& buffer, size_t & bufferOffset, size_t stringLength)
{
	if (stringLength == -1)
		stringLength = ReadBuffer<size_t>(buffer, bufferOffset);
	std::string str(buffer.data() + bufferOffset, buffer.data() + bufferOffset + sizeof(char) * stringLength);
	bufferOffset += sizeof(char) * stringLength;
	return str;
}
