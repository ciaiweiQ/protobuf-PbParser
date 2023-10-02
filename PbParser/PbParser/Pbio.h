#pragma once
#include "pbtool.h"
class PBIO_R
{
public:
	PBIO_R(const unsigned char* data, int len) 
	{
		buffer_ = data;
		buffer_end_ = data + len;
	};
	unsigned int ReadTag();
	BOOL ReadVarint64(uint64_t* value);
	BOOL ReadVarint32(uint32_t* value);
	BOOL ReadString(std::string* buffer, int size);
	BOOL Skip(int size);
	BOOL ReadInt64(uint64_t* value);
	BOOL ReadInt32(uint32_t* value);
	const unsigned char* buffer_;
	const unsigned char* buffer_end_;
};

class PBIO_W
{
public:
	void AddVarint(int number, uint64_t value);
	void AddFixed32(int number, uint32_t value);
	void AddFixed64(int number, uint64_t value);
	void AddLengthDelimited(int number, const char* data, int len);
	void AddGroup(int number, PBIO_W* Group);
	inline void Clear() 
	{
		m_NodeList.clear();
	};
	bool SerializeToString(std::string* output) const;

private:
	typedef struct PB_W_NODE 
	{	
		uint32_t number=0;
		uint32_t type=0;
		union
		{		
			uint64_t _Val64;
			uint32_t _Val32;
		
		} ;
		vector<char> data;
		PBIO_W* Group=0;
	};
	std::vector<PB_W_NODE> m_NodeList;

};

