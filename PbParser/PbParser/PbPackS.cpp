#include "pch.h"
#include "PbPackS.h"



PbPack::PbPack()
{

}

PbPack::~PbPack()
{

}
void PbPack::SetBin(int number, const char* data, int len)
{
	this->m_pb.AddLengthDelimited(number, data, len);
}
void PbPack::SetInt(int number, uint32_t val)
{
	this->m_pb.AddFixed32(number, val);
}
void PbPack::SetLong(int number, uint64_t val)
{
	this->m_pb.AddFixed64(number, val);
}
void PbPack::SetVarint(int number, uint64_t val)
{
	this->m_pb.AddVarint(number, val);
}
void PbPack::SetGroup(int number, PbPack* Group)
{
	this->m_pb.AddGroup(number, &(Group->m_pb));

}
void PbPack::Clear()
{
	this->m_pb.Clear();
}
int PbPack::Pack()
{
	m_buf.clear();
	this->m_pb.SerializeToString(&m_buf);
	return m_buf.size();
}
const char* PbPack::GetData()
{
	return this->m_buf.c_str();
}







PbPackTreeNode::PbPackTreeNode() 
{
	Data.clear();
}

PbPackTreeNode::~PbPackTreeNode()
{
	FreeALLNode();
	if (m_pb)
	{
		delete m_pb;
		m_pb = 0;
	}
}
PbPackTreeNode* PbPackTreeNode::FindNode(pbPath& _key, BOOL isArr, int arrindex)
{
	for (int i = 0; i < this->Data.size(); i++)
	{
		if (isArr)
		{
			if (this->Data[i]->path == _key && this->Data[i]->IsArr && this->Data[i]->ArrIndex == arrindex)
			{
				return this->Data[i]->Node;
			}
		}
		else
		{
			if (this->Data[i]->path == _key && !this->Data[i]->IsArr)
			{
				return this->Data[i]->Node;
			}
		}
	}
	return NULL;
}
void PbPackTreeNode::FreeALLNode()
{
	for (int i = 0; i < this->Data.size(); i++)
	{
		if (this->Data[i]->Node)
		{
			delete this->Data[i]->Node;
			this->Data[i]->Node = 0;
		}
		delete this->Data[i];
	}
	this->Data.clear();


}

int PbPackTreeNode::PackALLNode()
{
	if (!m_pb)m_pb = new PbPack;
	m_pb->Clear();
	for (int i = 0; i < this->Data.size(); i++)
	{
		if (this->Data[i]->Node)
		{
			int _len = this->Data[i]->Node->PackALLNode();
			m_pb->SetBin(this->Data[i]->path.number, this->Data[i]->Node->GetData(), _len);
		}
		else
		{
			switch (this->Data[i]->path.type)
			{
			case WIRETYPE_VARINT:
			{
				m_pb->SetVarint(this->Data[i]->path.number, this->Data[i]->Val);
			}
			break;
			case WIRETYPE_FIXED64:
			{
				m_pb->SetLong(this->Data[i]->path.number, this->Data[i]->Val);
			}
			break;
			case WIRETYPE_LENGTH_DELIMITED:
			{
				m_pb->SetBin(this->Data[i]->path.number, (char*)this->Data[i]->bytes.data(), this->Data[i]->bytes.size());
			}
			break;
			case WIRETYPE_FIXED32:
			{
				m_pb->SetInt(this->Data[i]->path.number, this->Data[i]->Val);
			}
			break;
			default:
				assert(0);
				return 0;
				break;
			}
		}

	}
	return this->m_pb->Pack();
}

const char* PbPackTreeNode::GetData()
{
	if (!this->m_pb)return 0;
	return this->m_pb->GetData();
}










PbPackS::PbPackS()
{

	this->m_RootTreeNode = new PbPackTreeNode();
}

PbPackS::~PbPackS()
{
	this->m_RootTreeNode->FreeALLNode();
	delete this->m_RootTreeNode;
}

void PbPackS::SetBin(const char* Key, const void* data, int len)
{
	PackSNode Node;
	Node.bytes = vector<unsigned char>((unsigned char*)data, ((unsigned char*)data) + len);
	Node.Val = 0;
	int puttype = this->NodePut(Node, Key);
	assert(puttype == WIRETYPE_LENGTH_DELIMITED);
}

void PbPackS::SetUTF(const char* Key, const char* val)
{
	string _utf = GBKToUTF8(val);
	this->SetBin(Key, _utf.c_str(), _utf.size());
}

void PbPackS::SetStr(const char* Key, const char* val)
{
	this->SetBin(Key, val, strlen(val));
}

void PbPackS::SetInt(const char* Key, uint32_t val)
{
	PackSNode Node;
	Node.bytes.clear();
	Node.Val = val;
	int puttype = this->NodePut(Node, Key);
	assert(puttype == WIRETYPE_FIXED32);
}
void PbPackS::SetInt64(const char* Key, uint64_t val)
{
	PackSNode Node;
	Node.bytes.clear();
	Node.Val = val;
	int puttype = this->NodePut(Node, Key);
	assert(puttype == WIRETYPE_FIXED64);
}
void PbPackS::SetVarInt(const char* Key, uint64_t val)
{
	PackSNode Node;
	Node.bytes.clear();
	Node.Val = val;
	int puttype = this->NodePut(Node, Key);
	assert(puttype == WIRETYPE_VARINT);
}

void PbPackS::SetZigZag32(const char* Key, int32_t val)
{
	SetVarInt(Key, ZigZagEncode32(val));
}

void PbPackS::SetZigZag64(const char* Key, int64_t val)
{
	SetVarInt(Key, ZigZagEncode64(val));
}

void PbPackS::SetBinArr(const char* Key, vector<vector<char>> val)
{

	for (int i = 0; i < val.size(); i++)
	{
		this->SetBin(Key, val[i].data(), val[i].size());
	}
}

void PbPackS::SetVarIntArr(const char* Key, vector<uint64_t> val)
{
	
	string keyStr = Key;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		keyStr = keys[keys.size() - 1];
	}
	pbPath Path;
	BOOL _isarr;
	int _arrindex;
	ParsKey(keyStr, &Path, &_isarr, &_arrindex);
	BOOL _IsProto2 = 1;
	if (Path.type== WIRETYPE_LENGTH_DELIMITED)
	{
		_IsProto2 = 0;
	}

	
	if (!_IsProto2)
	{
		int _mlen = val.size() * 10 + 10;
		char* _data = (char*)malloc(_mlen);
		if (_data)
		{
			memset(_data, 0, _mlen);
			int _len = 0;
			for (size_t i = 0; i < val.size(); i++)
			{
				_len = _len + WriteVarint((uint64_t)val[i], (unsigned char*)(_data + _len));
			}
			this->SetBin(Key, _data, _len);
			free(_data);
		}
	}
	else
	{
		for (int i = 0; i < val.size(); i++)
		{
			this->SetVarInt(Key, val[i]);
		}
	}



}
void PbPackS::SetVarInt32Arr(const char* Key, vector<uint32_t> val)
{
	
	string keyStr = Key;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		keyStr = keys[keys.size() - 1];
	}
	pbPath Path;
	BOOL _isarr;
	int _arrindex;
	ParsKey(keyStr, &Path, &_isarr, &_arrindex);
	BOOL _IsProto2 = 1;
	if (Path.type == WIRETYPE_LENGTH_DELIMITED)
	{
		_IsProto2 = 0;
	}
	if (!_IsProto2)
	{
		int _mlen = val.size() * 5 + 5;
		char* _data = (char*)malloc(_mlen);
		if (_data)
		{
			memset(_data, 0, _mlen);
			int _len = 0;
			for (size_t i = 0; i < val.size(); i++)
			{
				_len = _len + WriteVarint((uint64_t)val[i], (unsigned char*)(_data + _len));
			}
			this->SetBin(Key, _data, _len);
			free(_data);
		}
	}
	else
	{
		for (int i = 0; i < val.size(); i++)
		{
			this->SetVarInt(Key, val[i]);
		}
	}



}
void PbPackS::SetZigZag64Arr(const char* Key, vector<int64_t> val)
{
	string keyStr = Key;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		keyStr = keys[keys.size() - 1];
	}
	pbPath Path;
	BOOL _isarr;
	int _arrindex;
	ParsKey(keyStr, &Path, &_isarr, &_arrindex);
	BOOL _IsProto2 = 1;
	if (Path.type == WIRETYPE_LENGTH_DELIMITED)
	{
		_IsProto2 = 0;
	}
	
	if (!_IsProto2)
	{
		int _mlen = val.size() * 10 + 10;
		char* _data = (char*)malloc(_mlen);
		if (_data)
		{
			memset(_data, 0, _mlen);
			int _len = 0;
			for (size_t i = 0; i < val.size(); i++)
			{
				_len = _len + WriteVarint(ZigZagEncode64(val[i]), (unsigned char*)(_data + _len));
			}
			this->SetBin(Key, _data, _len);
			free(_data);
		}
	}
	else
	{
		for (int i = 0; i < val.size(); i++)
		{
			this->SetZigZag64(Key, val[i]);
		}
	}


}
void PbPackS::SetZigZag32Arr(const char* Key, vector<int32_t> val)
{
	
	string keyStr = Key;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		keyStr = keys[keys.size() - 1];
	}
	pbPath Path;
	BOOL _isarr;
	int _arrindex;
	ParsKey(keyStr, &Path, &_isarr, &_arrindex);
	BOOL _IsProto2 = 1;
	if (Path.type == WIRETYPE_LENGTH_DELIMITED)
	{
		_IsProto2 = 0;
	}
	if (!_IsProto2)
	{
		int _mlen = val.size() * 5 + 5;
		char* _data = (char*)malloc(_mlen);
		if (_data)
		{
			memset(_data, 0, _mlen);
			int _len = 0;
			for (size_t i = 0; i < val.size(); i++)
			{
				_len = _len + WriteVarint((uint64_t)ZigZagEncode32(val[i]), (unsigned char*)(_data + _len));
			}
			this->SetBin(Key, _data, _len);
			free(_data);
		}
	}
	else
	{
		for (int i = 0; i < val.size(); i++)
		{
			this->SetZigZag32(Key, val[i]);
		}
	}


}
void PbPackS::Clear()
{
	this->m_RootTreeNode->FreeALLNode();
	this->m_RootTreeNode->Data.clear();
}

int PbPackS::Pack()
{
	int len= this->m_RootTreeNode->PackALLNode();
	this->Clear();
	return len;
}

const char* PbPackS::GetData()
{
	return this->m_RootTreeNode->GetData();

}

int PbPackS::NodePut(PackSNode& Node, const char* Key)
{

	string keyStr = Key;
	pbPath Path;
	PbPackTreeNode* RootNode = this->m_RootTreeNode;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		for (int i = 0; i < keys.size() - 1; i++)
		{
			keyStr = keys[i];
			BOOL _isarr = 0;
			int _arrindex = 0;
			ParsKey(keyStr, &Path, &_isarr, &_arrindex);
			PbPackTreeNode* childNode = RootNode->FindNode(Path, _isarr, _arrindex);
			if (!childNode)
			{
				PackTreeNode* _Node = new PackTreeNode;
				_Node->path = Path;
				_Node->ArrIndex = _arrindex;
				_Node->IsArr = _isarr;
				_Node->Node = new PbPackTreeNode;
				childNode = _Node->Node;
				RootNode->Data.push_back(_Node);
			}
			RootNode = childNode;
		}
		keyStr = keys[keys.size() - 1];
	}

	BOOL _isarr = 0;
	int _arrindex = 0;
	ParsKey(keyStr, &Path, &_isarr, &_arrindex);
	PackTreeNode* _TreeNode = new PackTreeNode;
	_TreeNode->path = Path;
	_TreeNode->bytes = Node.bytes;
	_TreeNode->Val = Node.Val;
	_TreeNode->ArrIndex = _arrindex;
	_TreeNode->IsArr = _isarr;
	RootNode->Data.push_back(_TreeNode);
	return Path.type;
}
