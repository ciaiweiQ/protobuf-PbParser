#include "pch.h"
#include "Pbio.h"
#define kMaxVarintBytes 10
#define IO_W_MakeTag(number,type)	(((number) << 3) | (type))
unsigned int PBIO_R::ReadTag()
{
	int rlen = 0;
	uint32_t v = ::ReadVarint32((unsigned char*)buffer_, buffer_end_ - buffer_, &rlen);
	buffer_ += rlen;
	return v;
}

BOOL PBIO_R::ReadVarint64(uint64_t* value)
{
	
	int rlen = 0;
	value[0] = ReadVarint((unsigned char*)buffer_, buffer_end_ - buffer_, &rlen);
	buffer_ += rlen;
	
	return rlen!=0;
}

BOOL PBIO_R::ReadVarint32(uint32_t* value)
{
	int rlen = 0;
	value[0] = ::ReadVarint32((unsigned char*)buffer_, buffer_end_ - buffer_, &rlen);
	buffer_ += rlen;
	return rlen != 0;
}

BOOL PBIO_R::ReadString(std::string* buffer, int size)
{
	if (buffer_end_ - buffer_ >= size) {

		(buffer[0]).assign((char*)buffer_, size);
		buffer_ += size;
		return 1;
	}	
	return 0;
}

BOOL PBIO_R::Skip(int size)
{
	if (buffer_end_ - buffer_ >= size) 
	{
		buffer_ += size;
		return 1;
	}
	return 0;
}

BOOL PBIO_R::ReadInt64(uint64_t* value)
{
	if (buffer_end_ - buffer_>=sizeof(*value))
	{
		memcpy(value, buffer_, sizeof(*value));
		buffer_ += sizeof(*value);
		return 1;
	}
	return 0;
}

BOOL PBIO_R::ReadInt32(uint32_t* value)
{
	if (buffer_end_ - buffer_ >= sizeof(*value))
	{
		memcpy(value, buffer_, sizeof(*value));
		buffer_ += sizeof(*value);
		return 1;
	}
	return 0;

}



void PBIO_W::AddVarint(int number, uint64_t value)
{
	PB_W_NODE _node;
	_node.type = WIRETYPE_VARINT;
	_node._Val64 = value;
	_node.number = number;
	m_NodeList.push_back(_node);
}



void PBIO_W::AddFixed32(int number, uint32_t value)
{
	PB_W_NODE _node;
	_node.type = WIRETYPE_FIXED32;
	_node._Val32 = value;
	_node.number = number;
	m_NodeList.push_back(_node);
}

void PBIO_W::AddFixed64(int number, uint64_t value)
{
	PB_W_NODE _node;
	_node.type = WIRETYPE_FIXED64;
	_node._Val64 = value;
	_node.number = number;
	m_NodeList.push_back(_node);
}

void PBIO_W::AddLengthDelimited(int number, const char* data, int len)
{
	PB_W_NODE _node;
	_node.type = WIRETYPE_LENGTH_DELIMITED;
	_node.number = number;
	m_NodeList.push_back(_node);
	m_NodeList[m_NodeList.size() - 1].data.assign(data, data + len);
}

void PBIO_W::AddGroup(int number, PBIO_W* Group)
{
	PB_W_NODE _node;
	_node.type = WIRETYPE_START_GROUP;
	_node.number = number;
	_node.Group = Group;
	m_NodeList.push_back(_node);
}



bool PBIO_W::SerializeToString(std::string* output) const
{

	unsigned char bytes[10] = {0};
	int len = 0;
	for (int i = 0; i < m_NodeList.size(); i++)
	{
		switch (m_NodeList[i].type)
		{
		case WIRETYPE_VARINT:
		{
			
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, m_NodeList[i].type), bytes);
			output->append((char*)bytes, len);
			len = WriteVarint(m_NodeList[i]._Val64, bytes);
			output->append((char*)bytes, len);
		}		
			break;
		case WIRETYPE_FIXED64:
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, m_NodeList[i].type), bytes);
			output->append((char*)bytes, len);
			output->append((char*)&m_NodeList[i]._Val64, sizeof(m_NodeList[i]._Val64));
			break;
		case WIRETYPE_LENGTH_DELIMITED:
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, m_NodeList[i].type), bytes);
			output->append((char*)bytes, len);
			len = WriteVarint(m_NodeList[i].data.size(), bytes);
			output->append((char*)bytes, len);
			output->append((char*)m_NodeList[i].data.data(), m_NodeList[i].data.size());
			break;
		case WIRETYPE_FIXED32:
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, m_NodeList[i].type), bytes);
			output->append((char*)bytes, len);
			output->append((char*)&m_NodeList[i]._Val32, sizeof(m_NodeList[i]._Val32));
			break;
		case WIRETYPE_START_GROUP:
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, m_NodeList[i].type), bytes);
			output->append((char*)bytes, len);
			m_NodeList[i].Group->SerializeToString(output);
			len = WriteVarint(IO_W_MakeTag(m_NodeList[i].number, WIRETYPE_END_GROUP), bytes);
			output->append((char*)bytes, len);
			break;
		default:
			assert(0);
			return false;
			break;
		}
	}
	return true;
}
