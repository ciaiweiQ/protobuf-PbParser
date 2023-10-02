#include "pch.h"
#include "PbUnPack.h"




PbUnpackTreeNode::PbUnpackTreeNode()
{


}



PbUnpackTreeNode::~PbUnpackTreeNode()
{
	FreeALLNode();
}

PbUnpackTreeNode* PbUnpackTreeNode::FindPath(pbPath _key, BOOL isArr, int arrindex)
{

	for (int i = 0; i < Data.size(); i++)
	{
		if (isArr)
		{

			if (this->Data[i]->path == _key && this->Data[i]->IsUnPackArr && this->Data[i]->UnPackArrIndex == arrindex)
			{
				return this->Data[i]->Node;
			}
		}
		else
		{
			if (this->Data[i]->path == _key && !this->Data[i]->IsUnPackArr)
			{
				return this->Data[i]->Node;
			}
		}
	}




	return NULL;

}
void PbUnpackTreeNode::FreeALLNode()
{


	for (int i = 0; i < this->Data.size(); i++)
	{
		if (this->Data[i]->Node)
		{
			this->Data[i]->Node->FreeALLNode();
			delete this->Data[i]->Node;
		}
		delete this->Data[i];
	}
	this->Data.clear();


}
















PbUnPack::PbUnPack()
{
	this->m_RootTreeNode = new PbUnpackTreeNode();
	this->m_NodeCount = 0;
	m_Arrinfo.clear();
}


PbUnPack::~PbUnPack()
{
	this->Clear();
	this->m_RootTreeNode->FreeALLNode();
	delete this->m_RootTreeNode;
	

}

BOOL PbUnPack::UnPack(const void* data, int size)
{
	if (!this->m_NodeCount)return 0;
	PBIO_R input((unsigned char*)data, size);
	BOOL ret = SkipMessage(&input, this->m_RootTreeNode, "");

	for (int i = 0; i < m_GetObjInfo.size(); i++)
	{
		if (m_GetObjInfo[i].Freeaddr)
		{
			switch (m_GetObjInfo[i].type)
			{
			case GetType_UTF:
			{
				vector<char>* _addr = (vector<char>*)m_GetObjInfo[i].Freeaddr;
				string* _retaddr = (string*)m_GetObjInfo[i].retaddr;
				_retaddr[0] = UTF8ToGBK(_addr->data(), _addr->size());
			}
				break;
			case GetType_GBK:
			{
				vector<char>* _addr = (vector<char>*)m_GetObjInfo[i].Freeaddr;
				string* _retaddr = (string*)m_GetObjInfo[i].retaddr;
				_retaddr[0].assign (_addr->data(), _addr->size());
			}

				break;
			case GetType_UTFArr:
			{
				std::vector<vector<char>>* _addr = (std::vector<vector<char>>*)m_GetObjInfo[i].Freeaddr;
				vector<string>* _retaddr= (vector<string>*)m_GetObjInfo[i].retaddr;
				_retaddr->resize(_addr->size());
				for (int i = 0; i < _addr->size(); i++)
				{
					_retaddr[0][i] = UTF8ToGBK((_addr[0])[i].data(), (_addr[0])[i].size());
				}
			}
				break;
			case GetType_GBKArr:
			{
				std::vector<vector<char>>* _addr = (std::vector<vector<char>>*)m_GetObjInfo[i].Freeaddr;
				vector<string>* _retaddr = (vector<string>*)m_GetObjInfo[i].retaddr;
				_retaddr->resize(_addr->size());
				for (int i = 0; i < _addr->size(); i++)
				{
					_retaddr[0][i].assign((_addr[0])[i].data(), (_addr[0])[i].size());
				}
			}
			default:
				break;
			}
		}
	}
	this->Clear();
	return ret;
}

void PbUnPack::GetVarInt(const char* path, uint64_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT, Val, IsOk);
}
void PbUnPack::GetVarInt32(const char* path, uint32_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT32, Val, IsOk);
}
void PbUnPack::GetBinArr(const char* path, vector <vector<char>>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_BYTES_ARR, Val, IsOk);
}
void PbUnPack::GetInt64(const char* path, uint64_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_INT64, Val, IsOk);
}
void PbUnPack::GetInt64Arr(const char* path, vector<uint64_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_INT64_ARR, Val, IsOk);
}
void PbUnPack::GetInt(const char* path, uint32_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_INT32, Val, IsOk);
}
void PbUnPack::GetIntArr(const char* path, vector<uint32_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_INT32_ARR, Val, IsOk);
}
void PbUnPack::GetUTF(const char* path, std::string* Val, int* IsOk)
{
	GetObjInfo _info;
	_info.type = GetType_UTF;
	_info.Freeaddr = new vector<char>();
	_info.retaddr = Val;
	m_GetObjInfo.push_back(_info);
	this->NodePut(path, UNPACK_TYPE_BYTES, _info.Freeaddr, IsOk);
}
void PbUnPack::GetStr(const char* path, std::string* Val, int* IsOk)
{
	GetObjInfo _info;
	_info.type = GetType_GBK;
	_info.Freeaddr = new vector<char>();
	_info.retaddr = Val;
	m_GetObjInfo.push_back(_info);

	this->NodePut(path, UNPACK_TYPE_BYTES, _info.Freeaddr, IsOk);
}
void PbUnPack::GetUTFArr(const char* path, vector <std::string>* Val, int* IsOk)
{
	GetObjInfo _info;
	_info.type = GetType_UTFArr;
	_info.Freeaddr = new vector<vector<char>>();
	_info.retaddr = Val;
	m_GetObjInfo.push_back(_info);
	this->NodePut(path, UNPACK_TYPE_BYTES_ARR, _info.Freeaddr, IsOk);

}
void PbUnPack::GetStrArr(const char* path, vector <std::string>* Val, int* IsOk)
{
	GetObjInfo _info;
	_info.type = GetType_GBKArr;
	_info.Freeaddr = new vector<vector<char>>();
	_info.retaddr = Val;
	m_GetObjInfo.push_back(_info);
	this->NodePut(path, UNPACK_TYPE_BYTES_ARR, _info.Freeaddr, IsOk);
}
void PbUnPack::GetBin(const char* path, vector<char>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_BYTES, Val, IsOk);
}
void PbUnPack::GetVarIntArr(const char* path, vector<uint64_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT_ARR, Val, IsOk);
}
void PbUnPack::GetVarInt32Arr(const char* path, vector<uint32_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT32_ARR, Val, IsOk);
}
void PbUnPack::GetZigZag64(const char* path, int64_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT, Val, IsOk, 1);
}
void PbUnPack::GetZigZag64Arr(const char* path, vector<int64_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT_ARR, Val, IsOk, 1);
}
void PbUnPack::GetZigZag32(const char* path, int32_t* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT32, Val, IsOk, 1);
}
void PbUnPack::GetZigZag32Arr(const char* path, vector<int32_t>* Val, int* IsOk)
{
	this->NodePut(path, UNPACK_TYPE_VARINT32_ARR, Val, IsOk, 1);
}
void PbUnPack::Clear()
{
	this->m_NodeCount = 0;
	this->m_RootTreeNode->FreeALLNode();
	m_Arrinfo.clear();
	for (int i = 0; i < m_GetObjInfo.size(); i++)
	{
		if (m_GetObjInfo[i].Freeaddr)
		{
			switch (m_GetObjInfo[i].type)
			{
			case GetType_UTF:
			case GetType_GBK:
				delete ((vector<char>*)m_GetObjInfo[i].Freeaddr);
				m_GetObjInfo[i].Freeaddr = 0;
				break;
			case GetType_UTFArr:
			case GetType_GBKArr:
				delete ((vector<vector<char>>*)m_GetObjInfo[i].Freeaddr);
				m_GetObjInfo[i].Freeaddr = 0;
				break;
			default:
				break;
			}
		}
	}
	m_GetObjInfo.clear();
}

bool PbUnPack::SkipField(PBIO_R* input, uint32_t tag, PbUnpackTreeNode* TreeNode, string Layer)
{
	int number = tag >> 3;
	if (number == 0) return false;
	int WireType = tag & 7;
	pbPath path;
	path.number = number;
	path.type = WireType;

	string LayerPath;
	unsigned char bytes[5] = { 0 };
	char	KeyStrBytes[11] = { 0 };
	unsigned int Key = tag | WireType;
	int byteLen = WriteVarint(Key, bytes);
	ByteToHexStr(bytes, KeyStrBytes, byteLen);
	LayerPath = KeyStrBytes;
	if (!Layer.empty())
	{
		LayerPath = Layer + "." + LayerPath;
	}
	if (m_Arrinfo.count(LayerPath) == 0)
	{
		m_Arrinfo[LayerPath] = UnPackArr();
		m_Arrinfo[LayerPath].NodeList.clear();
		m_Arrinfo[LayerPath].IsArrStart = 0;
	}
	if (m_Arrinfo[LayerPath].IsArrStart)
	{
		if (!(path == m_Arrinfo[LayerPath].NodeList[m_Arrinfo[LayerPath].NodeList.size() - 1]))
		{
			m_Arrinfo[LayerPath].IsArrStart = 0;
			this->m_NodeCount--;
		}
	}
	m_Arrinfo[LayerPath].NodeList.push_back(path);

	switch (WireType)
	{
	case WIRETYPE_VARINT:
	{
		uint64_t value;
		if (!input->ReadVarint64(&value)) return false;
		int TypeList[] = { UNPACK_TYPE_VARINT ,UNPACK_TYPE_VARINT32,UNPACK_TYPE_VARINT_ARR,UNPACK_TYPE_VARINT32_ARR };
		for (int _lpos = 0; _lpos < sizeof(TypeList) / sizeof(TypeList[0]); _lpos++)
		{
			int UnPackType = TypeList[_lpos];
			for (int i = 0; i < TreeNode->Data.size(); i++)
			{
				BOOL _IsFindOk = 0;
				if (!TreeNode->Data[i]->Node && !TreeNode->Data[i]->IsOk && TreeNode->Data[i]->UnPackType == UnPackType && TreeNode->Data[i]->path == path)
				{
					if (TreeNode->Data[i]->IsUnPackArr)
					{
						if (GetLayerNodeCount(LayerPath, path) == TreeNode->Data[i]->UnPackArrIndex + 1)
						{
							_IsFindOk = 1;
						}
					}
					else
					{
						_IsFindOk = 1;
					}
				}
				if (_IsFindOk)
				{
					switch (UnPackType)
					{
					case UNPACK_TYPE_VARINT:

						TreeNode->Data[i]->_Val64[0] = TreeNode->Data[i]->IsZigZag ? ZigZagDecode64(value): value;
						TreeNode->Data[i]->IsOk = 1;
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						this->m_NodeCount--;
						break;
					case UNPACK_TYPE_VARINT32:
						TreeNode->Data[i]->_Val32[0] = TreeNode->Data[i]->IsZigZag ? ZigZagDecode32((uint32_t)value) : (uint32_t)value;
						TreeNode->Data[i]->IsOk = 1;
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						this->m_NodeCount--;
						break;
					case UNPACK_TYPE_VARINT_ARR:

						TreeNode->Data[i]->_Val64Arr[0].push_back(TreeNode->Data[i]->IsZigZag ? ZigZagDecode64(value) : value);
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						if (!m_Arrinfo[LayerPath].IsArrStart)m_Arrinfo[LayerPath].IsArrStart = 1;

						break;
					case UNPACK_TYPE_VARINT32_ARR:
						TreeNode->Data[i]->_Val32Arr[0].push_back(TreeNode->Data[i]->IsZigZag ? ZigZagDecode32((uint32_t)value) : (uint32_t)value);
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						if (!m_Arrinfo[LayerPath].IsArrStart)m_Arrinfo[LayerPath].IsArrStart = 1;
						break;
					default:
						assert(0);
						break;
					}
					break;
				}
			}
		}
		return true;
	}
	case WIRETYPE_FIXED64:
	{
		uint64_t value;
		if (!input->ReadInt64(&value)) return false;

		int TypeList[] = { UNPACK_TYPE_INT64 ,UNPACK_TYPE_INT64_ARR };
		for (int _lpos = 0; _lpos < sizeof(TypeList) / sizeof(TypeList[0]); _lpos++)
		{
			int UnPackType = TypeList[_lpos];
			for (int i = 0; i < TreeNode->Data.size(); i++)
			{
				BOOL _IsFindOk = 0;
				if (!TreeNode->Data[i]->Node && !TreeNode->Data[i]->IsOk && TreeNode->Data[i]->UnPackType == UnPackType && TreeNode->Data[i]->path == path)
				{
					if (TreeNode->Data[i]->IsUnPackArr)
					{
						if (GetLayerNodeCount(LayerPath, path) == TreeNode->Data[i]->UnPackArrIndex + 1)
						{
							_IsFindOk = 1;
						}
					}
					else
					{
						_IsFindOk = 1;
					}
				}
				if (_IsFindOk)
				{
					switch (UnPackType)
					{
					case UNPACK_TYPE_INT64:
						TreeNode->Data[i]->_Val64[0] = value;
						TreeNode->Data[i]->IsOk = 1;
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						this->m_NodeCount--;
						break;

					case UNPACK_TYPE_INT64_ARR:
						TreeNode->Data[i]->_Val64Arr[0].push_back(value);
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						if (!m_Arrinfo[LayerPath].IsArrStart)m_Arrinfo[LayerPath].IsArrStart = 1;
						break;

					default:
						assert(0);
						break;
					}
					break;
				}
			}
		}

		return true;
	}
	case WIRETYPE_LENGTH_DELIMITED:
	{
		uint32_t length;
		if (!input->ReadVarint32(&length)) return false;
		BOOL IsRead = 0;
		string value = "";
		int TypeList[] = { UNPACK_TYPE_BYTES ,UNPACK_TYPE_BYTES_ARR,UNPACK_TYPE_VARINT_ARR ,UNPACK_TYPE_VARINT32_ARR };
		for (int _lpos = 0; _lpos < sizeof(TypeList) / sizeof(TypeList[0]); _lpos++)
		{
			int UnPackType = TypeList[_lpos];
			for (int i = 0; i < TreeNode->Data.size(); i++)
			{
				BOOL _IsFindOk = 0;
				if (!TreeNode->Data[i]->Node && !TreeNode->Data[i]->IsOk && TreeNode->Data[i]->UnPackType == UnPackType && TreeNode->Data[i]->path == path)
				{
					if (TreeNode->Data[i]->IsUnPackArr)
					{
						if (GetLayerNodeCount(LayerPath, path) == TreeNode->Data[i]->UnPackArrIndex + 1)
						{
							_IsFindOk = 1;
						}
					}
					else
					{
						_IsFindOk = 1;
					}
				}
				if (_IsFindOk)
				{
					if (!IsRead)
					{
						if (!input->ReadString(&value, length))return false;
						IsRead = 1;
					}

					switch (UnPackType)
					{
					case UNPACK_TYPE_BYTES:
						TreeNode->Data[i]->_ValBin->assign(value.begin(), value.end());
						TreeNode->Data[i]->IsOk = 1;
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						this->m_NodeCount--;
						break;

					case UNPACK_TYPE_BYTES_ARR:
						TreeNode->Data[i]->_ValBinArr[0].push_back(vector<char>(value.begin(), value.end()));
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						if (!m_Arrinfo[LayerPath].IsArrStart)m_Arrinfo[LayerPath].IsArrStart = 1;
						break;
					case UNPACK_TYPE_VARINT_ARR:
					{
						vector<uint64_t> intArr = ReadVarintArr((unsigned char*)value.c_str(), value.length());
						if (intArr.size() > 0)
						{
							if (TreeNode->Data[i]->IsZigZag)
							{
								for (int ii = 0; ii < intArr.size(); ii++)
								{
									TreeNode->Data[i]->_Val64Arr[0].push_back( ZigZagDecode64(intArr[ii]));
								}
							}
							else
							{
								TreeNode->Data[i]->_Val64Arr->assign(intArr.begin(), intArr.end());
							}
							
							if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
							this->m_NodeCount--;
						}
					}
					break;
					case UNPACK_TYPE_VARINT32_ARR:
					{
						vector<uint64_t> intArr = ReadVarintArr((unsigned char*)value.c_str(), value.length());
						if (intArr.size() > 0)
						{
							for (int ii = 0; ii < intArr.size(); ii++)
							{
								TreeNode->Data[i]->_Val32Arr->push_back(TreeNode->Data[i]->IsZigZag ? ZigZagDecode32((uint32_t)intArr[ii]) : (uint32_t)intArr[ii]);
							}
							if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
							this->m_NodeCount--;
						}
					}
					break;
					default:
						assert(0);
						break;
					}
					break;
				}
			}
		}


		for (int j = 0; j < TreeNode->Data.size(); j++)
		{
			if (!TreeNode->Data[j]->IsOk && TreeNode->Data[j]->Node && TreeNode->Data[j]->path == path)
			{

				BOOL _IsFindOk = 0;
				if (TreeNode->Data[j]->IsUnPackArr)
				{
					if (GetLayerNodeCount(LayerPath, path) == TreeNode->Data[j]->UnPackArrIndex + 1)
					{
						_IsFindOk = 1;
					}
				}
				else
				{
					_IsFindOk = 1;
				}
				if (_IsFindOk)
				{
					if (!IsRead)
					{
						if (!input->ReadString(&value, length))return false;
						IsRead = 1;
					}
					PBIO_R _input((unsigned char*)value.c_str(), value.size());
					SkipMessage(&_input, TreeNode->Data[j]->Node, LayerPath);
				}
			}
		}
		if (!IsRead)
		{
			if (!input->Skip(length)) return false;
			IsRead = 1;
		}
		return true;
	}
	case WIRETYPE_START_GROUP: {

		if (!SkipMessage(input, TreeNode, Layer)) {
			return false;
		}
		return true;
	}
	case WIRETYPE_END_GROUP: {
		return false;
	}
	case WIRETYPE_FIXED32: {
		uint32_t value;
		if (!input->ReadInt32(&value)) return false;

		int TypeList[] = { UNPACK_TYPE_INT32 ,UNPACK_TYPE_INT32_ARR };
		for (int _lpos = 0; _lpos < sizeof(TypeList) / sizeof(TypeList[0]); _lpos++)
		{
			int UnPackType = TypeList[_lpos];
			for (int i = 0; i < TreeNode->Data.size(); i++)
			{
				BOOL _IsFindOk = 0;
				if (!TreeNode->Data[i]->Node && !TreeNode->Data[i]->IsOk && TreeNode->Data[i]->UnPackType == UnPackType && TreeNode->Data[i]->path == path)
				{
					if (TreeNode->Data[i]->IsUnPackArr)
					{
						if (GetLayerNodeCount(LayerPath, path) == TreeNode->Data[i]->UnPackArrIndex + 1)
						{
							_IsFindOk = 1;
						}
					}
					else
					{
						_IsFindOk = 1;
					}
				}
				if (_IsFindOk)
				{
					switch (UnPackType)
					{
					case UNPACK_TYPE_INT32:
						TreeNode->Data[i]->_Val32[0] = value;
						TreeNode->Data[i]->IsOk = 1;
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						this->m_NodeCount--;
						break;

					case UNPACK_TYPE_INT32_ARR:
						TreeNode->Data[i]->_Val32Arr[0].push_back(value);
						if (TreeNode->Data[i]->_ValIsOk)TreeNode->Data[i]->_ValIsOk[0] = 1;
						if (!m_Arrinfo[LayerPath].IsArrStart)m_Arrinfo[LayerPath].IsArrStart = 1;
						break;

					default:
						assert(0);
						break;
					}
					break;
				}
			}
		}
		return true;
	}
	default: {
		return false;
	}
	}
	return false;
}
BOOL PbUnPack::SkipMessage(PBIO_R* input, PbUnpackTreeNode* TreeNode, string Layer)
{
	while (true)
	{
		if (this->m_NodeCount <= 0)
		{
			return true;
		}
		uint32_t tag = input->ReadTag();
		if (tag == 0)
		{
			if (input->buffer_ == input->buffer_end_)
			{
				// End of input.  This is a valid place to end, so return true.
				return true;
			}
			else
			{
				return false;
			}
		}
		if ((int)(tag & 7) == WIRETYPE_END_GROUP) {
			// Must be the end of the message.
			return true;
		}
		if (!SkipField(input, tag, TreeNode, Layer)) return false;
	}
	return false;
}
void PbUnPack::NodePut(const char* Key, int type, void* addr, int* Isok, BOOL IsZigZag)
{
	if (!Key || !addr)return;
	unsigned char keyData[10] = { 0 };
	string keyStr;
	pbPath Path;
	PbUnpackTreeNode* RootNode = this->m_RootTreeNode;
	keyStr = Key;
	if ((int)keyStr.find(".") > -1)
	{
		vector<string> keys = StringSplit(keyStr, ".");
		for (int i = 0; i < keys.size() - 1; i++)
		{
			keyStr = keys[i];
			BOOL _isarr = 0;
			int _arrindex = 0;
			ParsKey(keyStr, &Path, &_isarr, &_arrindex);
			PbUnpackTreeNode* childNode = RootNode->FindPath(Path, _isarr, _arrindex);
			if (!childNode)
			{

				UnPackTreeNode* _Node = new UnPackTreeNode;
				_Node->path = Path;
				_Node->UnPackArrIndex = _arrindex;
				_Node->IsUnPackArr = _isarr;
				_Node->Node = new PbUnpackTreeNode;
				_Node->UnPackType = type;
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
	UnPackTreeNode* _TreeNode = new UnPackTreeNode;
	_TreeNode->IsOk = 0;
	_TreeNode->path = Path;
	_TreeNode->UnPackType = type;
	_TreeNode->UnPackArrIndex = _arrindex;
	_TreeNode->IsUnPackArr = _isarr;
	_TreeNode->_ValIsOk = Isok;
	_TreeNode->IsZigZag = IsZigZag;
	if (Isok)Isok[0] = 0;
	switch (type)
	{
	case UNPACK_TYPE_BYTES_ARR:
		_TreeNode->_ValBinArr = (vector<vector<char>>*)addr;
		_TreeNode->_ValBinArr->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_BYTES:
		_TreeNode->_ValBin = (vector<char>*)addr;
		_TreeNode->_ValBin->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_VARINT:
		_TreeNode->_Val64 = (uint64_t*)addr;
		_TreeNode->_Val64[0] = 0;
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_INT32:
		_TreeNode->_Val32 = (uint32_t*)addr;
		_TreeNode->_Val32[0] = 0;
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_INT64:
		_TreeNode->_Val64 = (uint64_t*)addr;
		_TreeNode->_Val64[0] = 0;
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_VARINT_ARR:
		_TreeNode->_Val64Arr = (vector<uint64_t>*)addr;
		_TreeNode->_Val64Arr->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_INT32_ARR:
		_TreeNode->_Val32Arr = (vector<uint32_t>*)addr;
		_TreeNode->_Val32Arr->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_INT64_ARR:
		_TreeNode->_Val64Arr = (vector<uint64_t>*)addr;
		_TreeNode->_Val64Arr->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_VARINT32:
		_TreeNode->_Val32 = (uint32_t*)addr;
		_TreeNode->_Val32[0] = 0;
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	case UNPACK_TYPE_VARINT32_ARR:
		_TreeNode->_Val32Arr = (vector<uint32_t>*)addr;
		_TreeNode->_Val32Arr->clear();
		this->m_NodeCount++;
		RootNode->Data.push_back(_TreeNode);
		break;
	default:
		assert(0);
		break;
	}

}

int PbUnPack::GetLayerNodeCount(string Layer, pbPath path)
{
	if (m_Arrinfo.count(Layer) == 0)return 0;
	int retcount = 0;
	for (int i = 0; i < m_Arrinfo[Layer].NodeList.size(); i++)
	{
		if (m_Arrinfo[Layer].NodeList[i] == path)
		{
			retcount++;
		}
	}
	return retcount;
}


