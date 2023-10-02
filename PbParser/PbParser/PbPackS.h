#ifndef  __PB_PACKS_H
#define __PB_PACKS_H


#include "Pbio.h"
class PbPack
{
public:
	PbPack();
	~PbPack();
	void SetBin(int number, const char* data, int len);
	void SetInt(int number, uint32_t val);
	void SetLong(int number, uint64_t val);
	void SetVarint(int number, uint64_t val);
	//已废弃  一般用不到
	void SetGroup(int number, PbPack* Group);
	void Clear();
	int Pack();
	const char* GetData();
private:

	PBIO_W m_pb;
	string m_buf;

};


class PbPackTreeNode;
typedef struct PackTreeNode {
	pbPath path = {};
	uint64_t Val=0;
	vector<unsigned char> bytes;
	BOOL IsArr = 0;
	int ArrIndex = 0;
	PbPackTreeNode* Node = 0;
};
class PbPackTreeNode
{
public:
	PbPackTreeNode();
	~PbPackTreeNode();
	vector <PackTreeNode*> Data;
	PbPackTreeNode* FindNode(pbPath& _key, BOOL isArr, int arrindex);
	void FreeALLNode();
	int PackALLNode();
	const char* GetData();
private:
	PbPack* m_pb=0;
};

typedef struct
{
	uint64_t Val;
	vector<unsigned char> bytes;
} PackSNode;
class PbPackS
{
public:
	PbPackS();
	~PbPackS();
	void SetBin(const char* Key, const void* data, int len);
	void SetUTF(const char* Key, const char* val);
	void SetStr(const char* Key, const char* val);
	void SetInt(const char* Key, uint32_t val);
	void SetInt64(const char* Key, uint64_t val);
	void SetVarInt(const char* Key, uint64_t val);
	void SetZigZag32(const char* Key, int32_t val);
	void SetZigZag64(const char* Key, int64_t val);
	void Clear();
	int Pack();
	const char* GetData();


	void SetBinArr(const char* Key, vector<vector<char>> val);
	void SetVarIntArr(const char* Key, vector<uint64_t> val);


	void SetVarInt32Arr(const char* Key, vector<uint32_t> val);

	void SetZigZag64Arr(const char* Key, vector<int64_t> val);
	void SetZigZag32Arr(const char* Key, vector<int32_t> val);


private:
	int NodePut(PackSNode& Node, const char* Key);
	PbPackTreeNode* m_RootTreeNode;

};

#endif // ! __PB_PACKS_H




