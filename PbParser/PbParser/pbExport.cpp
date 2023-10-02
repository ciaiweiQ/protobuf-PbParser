#include "pch.h"
#ifdef _USRDLL

#include "pbExport.h"
vector<char>* pb_vector_char_New()
{
	return new vector<char>();
}
void pb_vector_char_Free(vector<char>* thisPtr)
{
	delete thisPtr;
}
vector<vector<char>>* pb_vector_vector_char_New()
{
	return new vector<vector<char>>();
}
void pb_vector_vector_char_Free(vector<vector<char>>* thisPtr)
{
	delete thisPtr;
}
int pb_vector_vector_char_size(vector<vector<char>>* thisPtr)
{
	return thisPtr->size();
}
char* pb_vector_char_Getdata(vector<char>* thisPtr, int* retlen)
{
	retlen[0] = thisPtr->size();
	return thisPtr->data();
}
char* pb_vector_vector_char_Getdata(vector<vector<char>>* thisPtr, int index, int* retlen)
{
	retlen[0] = (thisPtr[0])[index].size();
	return (thisPtr[0])[index].data();
}

vector<uint64_t>* pb_vector_long_New()
{
	return new vector<uint64_t>();
}
void pb_vector_long_Free(vector<uint64_t>* thisPtr)
{
	delete thisPtr;
}
uint64_t* pb_vector_long_Getdata(vector<uint64_t>* thisPtr, int* retlen)
{
	retlen[0] = thisPtr->size();
	return thisPtr->data();
}
vector<uint32_t>* pb_vector_Int_New()
{
	return new vector<uint32_t>();
}
void pb_vector_Int_Free(vector<uint32_t>* thisPtr)
{
	delete thisPtr;
}
uint32_t* pb_vector_Int_Getdata(vector<uint32_t>* thisPtr, int* retlen)
{
	retlen[0] = thisPtr->size();
	return thisPtr->data();
}
BOOL pb_KeyGetTypeAndNumber(const char* key, int* Type, int* Number)
{
	string keyStr;
	keyStr = key;
	if ((int)keyStr.find(".") > -1)return 0;

	pbPath path;
	BOOL isArr;
	int arrIndex;
	ParsKey(keyStr, &path, &isArr, &arrIndex);
	Number[0] = path.number;
	Type[0] = path.type;
	if (Number[0] == 0)return 0;
	if (Type[0] < 0 || Type[0] >5)return 0;
	return 1;
}
uint64_t pb_ReadVarint(unsigned char* data, int len, int* readLen)
{
	return ReadVarint(data, len, readLen);
}
uint32_t pb_ReadVarint32(unsigned char* data, int len, int* readLen)
{
	return ReadVarint32(data, len, readLen);
}
int pb_WriteVarint(uint64_t value, unsigned char* ptr)
{
	return WriteVarint(value, ptr);
}
uint32_t pb_ZigZagEncode32(int32_t n)
{
	return ZigZagEncode32(n);
}
int32_t pb_ZigZagDecode32(uint32_t n)
{
	return ZigZagDecode32(n);
}
uint64_t pb_ZigZagEncode64(int64_t n)
{
	return ZigZagEncode64(n);
}
int64_t pb_ZigZagDecode64(uint64_t n)
{
	return ZigZagDecode64(n);
}








const char* pbUtil_GetPackCode(pbUtil* thisptr, char* data, int len)
{
	return thisptr->GetPackCode(data, len);
}
pbUtil* pbUtil_New()
{
	return new pbUtil;
}
void pbUtil_Free(pbUtil* thisptr)
{
	delete thisptr;
}




void PbPackS_SetBin(PbPackS* thisptr, const char* Key, const void* data, int len)
{
	return thisptr->SetBin(Key, data, len);
}

void PbPackS_SetInt(PbPackS* thisptr, const char* Key, uint32_t val)
{
	return thisptr->SetInt(Key, val);
}
void PbPackS_SetLong(PbPackS* thisptr, const char* Key, uint64_t val)
{
	return thisptr->SetInt64(Key, val);
}
void PbPackS_SetVarInt(PbPackS* thisptr, const char* Key, uint64_t val)
{
	return thisptr->SetVarInt(Key, val);
}
void PbPackS_SetZigZag32(PbPackS* thisptr, const char* Key, int32_t val)
{
	return thisptr->SetZigZag32(Key, val);
}
void PbPackS_SetZigZag64(PbPackS* thisptr, const char* Key, int64_t val)
{
	return thisptr->SetZigZag64(Key, val);
}
void PbPackS_Clear(PbPackS* thisptr)
{
	return thisptr->Clear();
}
int PbPackS_Pack(PbPackS* thisptr)
{
	return thisptr->Pack();
}
const char* PbPackS_GetData(PbPackS* thisptr)
{
	return thisptr->GetData();
}

PbPackS* PbPackS_New()
{
	return new PbPackS;
}
void PbPackS_Free(PbPackS* thisptr)
{
	delete thisptr;
}
BOOL PbUnPack_UnPack(PbUnPack* thisptr, const void* data, int size)
{
	return thisptr->UnPack(data, size);
}
void PbUnPack_GetVarInt(PbUnPack* thisptr, const char* path, uint64_t* Val, int* IsOk)
{
	return thisptr->GetVarInt(path, Val, IsOk);
}
void PbUnPack_GetVarIntArr(PbUnPack* thisptr, const char* path, vector<uint64_t>* Val, int* IsOk)
{
	return thisptr->GetVarIntArr(path, Val, IsOk);
}
void PbUnPack_GetVarInt32(PbUnPack* thisptr, const char* path, uint32_t* Val, int* IsOk)
{
	return thisptr->GetVarInt32(path, Val, IsOk);
}
void PbUnPack_GetVarInt32Arr(PbUnPack* thisptr, const char* path, vector<uint32_t>* Val, int* IsOk)
{
	return thisptr->GetVarInt32Arr(path, Val, IsOk);
}
void PbUnPack_GetZigZag64(PbUnPack* thisptr, const char* path, int64_t* Val, int* IsOk)
{
	return thisptr->GetZigZag64(path, Val, IsOk);
}
void PbUnPack_GetZigZag64Arr(PbUnPack* thisptr, const char* path, vector<int64_t>* Val, int* IsOk)
{
	return thisptr->GetZigZag64Arr(path, Val, IsOk);
}
void PbUnPack_GetZigZag32(PbUnPack* thisptr, const char* path, int32_t* Val, int* IsOk)
{
	return thisptr->GetZigZag32(path, Val, IsOk);
}
void PbUnPack_GetZigZag32Arr(PbUnPack* thisptr, const char* path, vector<int32_t>* Val, int* IsOk)
{
	return thisptr->GetZigZag32Arr(path, Val, IsOk);
}
void PbUnPack_GetBin(PbUnPack* thisptr, const char* path, vector<char>* Val, int* IsOk)
{
	return thisptr->GetBin(path, Val, IsOk);
}
void PbUnPack_GetBinArr(PbUnPack* thisptr, const char* path, vector<vector<char>>* Val, int* IsOk)
{
	return thisptr->GetBinArr(path, Val, IsOk);
}
void PbUnPack_GetInt64(PbUnPack* thisptr, const char* path, uint64_t* Val, int* IsOk)
{
	return thisptr->GetInt64(path, Val, IsOk);
}
void PbUnPack_GetInt64Arr(PbUnPack* thisptr, const char* path, vector<uint64_t>* Val, int* IsOk)
{
	return thisptr->GetInt64Arr(path, Val, IsOk);
}
void PbUnPack_GetInt(PbUnPack* thisptr, const char* path, uint32_t* Val, int* IsOk)
{
	return thisptr->GetInt(path, Val, IsOk);
}
void PbUnPack_GetIntArr(PbUnPack* thisptr, const char* path, vector <uint32_t>* Val, int* IsOk)
{
	return thisptr->GetIntArr(path, Val, IsOk);
}
void PbUnPack_Clear(PbUnPack* thisptr)
{
	return thisptr->Clear();
}
PbUnPack* PbUnPack_New()
{
	return new PbUnPack;
}
void PbUnPack_Free(PbUnPack* thisptr)
{
	delete thisptr;
}



#endif // _USRDLL
