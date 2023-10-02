#ifndef __PB_UNPACK__H
#define __PB_UNPACK__H
#include "pbtool.h"
#include "Pbio.h"
#include <map>

#define PROTOBUF_WIRE_FORMAT_MAKE_TAG(FIELD_NUMBER, TYPE) \
  static_cast<uint32_t>((static_cast<uint32_t>(FIELD_NUMBER) << 3) | (TYPE))



enum UNPACKTYPE 
{
	//bytes 数组
	UNPACK_TYPE_BYTES_ARR,
	//bytes
	UNPACK_TYPE_BYTES,
	//varint
	UNPACK_TYPE_VARINT,
	//FIXED32 
	UNPACK_TYPE_INT32,
	//FIXED64
	UNPACK_TYPE_INT64,
	//varint 数组
	UNPACK_TYPE_VARINT_ARR,
	//FIXED32 数组
	UNPACK_TYPE_INT32_ARR,
	//FIXED64 数组
	UNPACK_TYPE_INT64_ARR,
	//varint 32位
	UNPACK_TYPE_VARINT32,
	//varint 32位数组
	UNPACK_TYPE_VARINT32_ARR,


	UNPACKTYPE_MAX,
};


class PbUnpackTreeNode;
typedef struct UnPackTreeNode 
{
	pbPath path = {};
	int UnPackType = 0;
	BOOL IsUnPackArr = 0;
	int UnPackArrIndex = 0;

	PbUnpackTreeNode* Node = 0;
	BOOL IsOk = 0;
	BOOL IsZigZag = 0;
	union
	{
		uint64_t* _Val64 = 0;
		uint32_t* _Val32;
		vector<vector<char>>* _ValBinArr;
		vector<char>* _ValBin;
		vector<uint64_t>* _Val64Arr;
		vector<uint32_t>* _Val32Arr;
	};
	int* _ValIsOk = 0;
};

class PbUnpackTreeNode
{
public:
	PbUnpackTreeNode();
	~PbUnpackTreeNode();
	vector <UnPackTreeNode*> Data;
	PbUnpackTreeNode* FindPath(pbPath _key, BOOL isArr, int arrindex);
	void FreeALLNode();

private:

};

typedef struct UnPackArr 
{
	std::vector<pbPath> NodeList;
	BOOL IsArrStart = 0;

};

class PbUnPack
{
public:
	PbUnPack();
	~PbUnPack();
	BOOL UnPack(const void* data,  int size);
	void GetVarInt(const char* path,uint64_t* Val,int* IsOk=NULL);
	void GetVarIntArr(const char* path, vector<uint64_t>* Val, int* IsOk = NULL);
	void GetVarInt32(const char* path, uint32_t* Val, int* IsOk = NULL);
	void GetVarInt32Arr(const char* path, vector<uint32_t>* Val, int* IsOk = NULL);

	void GetZigZag64(const char* path, int64_t* Val, int* IsOk = NULL);
	void GetZigZag64Arr(const char* path, vector<int64_t>* Val, int* IsOk = NULL);
	void GetZigZag32(const char* path, int32_t* Val, int* IsOk = NULL);
	void GetZigZag32Arr(const char* path, vector<int32_t>* Val, int* IsOk = NULL);


	void GetBin(const char* path, vector<char>* Val, int* IsOk = NULL);
	void GetBinArr(const char* path, vector<vector<char>>* Val, int* IsOk = NULL);
	void GetInt64(const char* path, uint64_t* Val, int* IsOk = NULL);
	void GetInt64Arr(const char* path, vector<uint64_t>* Val, int* IsOk = NULL);
	void GetInt(const char* path, uint32_t* Val, int* IsOk = NULL);
	void GetIntArr(const char* path, vector <uint32_t>* Val, int* IsOk = NULL);

	void GetUTF(const char* path, std::string* Val, int* IsOk = NULL);
	void GetStr(const char* path, std::string* Val, int* IsOk = NULL);
	void GetUTFArr(const char* path, vector <std::string>* Val, int* IsOk = NULL);
	void GetStrArr(const char* path, vector <std::string>* Val, int* IsOk = NULL);

	void Clear();
private:

	enum GetType
	{
		GetType_UTF = 1,
		GetType_GBK = 2,
		GetType_UTFArr = 3,
		GetType_GBKArr = 4,
	};
	typedef struct GetObjInfo 
	{
		GetType type;
		void* retaddr = 0;
		void* Freeaddr = 0;
	};
	bool SkipField(PBIO_R* input, uint32_t tag, PbUnpackTreeNode* TreeNode, string Layer);
	BOOL SkipMessage(PBIO_R* input, PbUnpackTreeNode* TreeNode,string Layer);
	void NodePut(const char* Key,int type,void* addr,int* Isok,BOOL IsZigZag=0);
	int GetLayerNodeCount(string Layer, pbPath path);

	int m_NodeCount=0;
	PbUnpackTreeNode* m_RootTreeNode=0;
	std::map<string, UnPackArr> m_Arrinfo;
	std::vector<GetObjInfo> m_GetObjInfo;
};



#endif // !__PB_UNPACK__H

