#pragma once

#include "MapleStructs.h"

class MaplePacket
{
	std::string m_error;
	std::string m_source;
	std::vector<uint8_t> m_data;
	bool m_bShouldBeParsed;

public:
	MaplePacket();
	bool IsConnected();

	void Encode1(uint8_t data);
	void Encode2(uint16_t data);
	void Encode4(uint32_t data);
	void Encode8(uint64_t data);
	void EncodeString(std::string data);

	bool Parse(std::string& source);
	bool Recv();
	bool Send();

	std::string ToString();
	std::string GetError();
};