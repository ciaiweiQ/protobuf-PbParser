#pragma once
#include "pbtool.h"
#include "pbUtil.h"
#include "PbPackS.h"
#include "PbUnPack.h"
#ifdef _USRDLL
#define DLLEXPORT __declspec(dllexport)
#ifdef __cplusplus
extern "C" {
#endif

	DLLEXPORT vector<char>* pb_vector_char_New();
	DLLEXPORT void pb_vector_char_Free(vector<char>* thisPtr);
	DLLEXPORT char* pb_vector_char_Getdata(vector<char>* thisPtr, int* retlen);
	DLLEXPORT vector<vector<char>>* pb_vector_vector_char_New();
	DLLEXPORT void pb_vector_vector_char_Free(vector<vector<char>>* thisPtr);
	DLLEXPORT int pb_vector_vector_char_size(vector<vector<char>>* thisPtr);
	DLLEXPORT char* pb_vector_vector_char_Getdata(vector<vector<char>>* thisPtr, int index, int* retlen);
	DLLEXPORT vector<uint64_t>* pb_vector_long_New();
	DLLEXPORT void pb_vector_long_Free(vector<uint64_t>* thisPtr);
	DLLEXPORT uint64_t* pb_vector_long_Getdata(vector<uint64_t>* thisPtr, int* retlen);
	DLLEXPORT vector<uint32_t>* pb_vector_Int_New();
	DLLEXPORT void pb_vector_Int_Free(vector<uint32_t>* thisPtr);


	DLLEXPORT uint32_t* pb_vector_Int_Getdata(vector<uint32_t>* thisPtr, int* retlen);
	DLLEXPORT BOOL pb_KeyGetTypeAndNumber(const char* key, int* Type, int* Number);
	DLLEXPORT uint64_t pb_ReadVarint(unsigned char* data, int len, int* readLen = NULL);
	DLLEXPORT uint32_t pb_ReadVarint32(unsigned char* data, int len, int* readLen = NULL);
	DLLEXPORT int pb_WriteVarint(uint64_t value, unsigned char* ptr);
	DLLEXPORT uint32_t pb_ZigZagEncode32(int32_t n);
	DLLEXPORT int32_t pb_ZigZagDecode32(uint32_t n);
	DLLEXPORT uint64_t pb_ZigZagEncode64(int64_t n);
	DLLEXPORT int64_t pb_ZigZagDecode64(uint64_t n);



	DLLEXPORT const char* pbUtil_GetPackCode(pbUtil* thisptr, char* data, int len);
	DLLEXPORT pbUtil* pbUtil_New();
	DLLEXPORT void pbUtil_Free(pbUtil* thisptr);

	DLLEXPORT void PbPackS_SetBin(PbPackS* thisptr, const char* Key, const void* data, int len);

	DLLEXPORT void PbPackS_SetInt(PbPackS* thisptr, const char* Key, uint32_t val);
	DLLEXPORT void PbPackS_SetLong(PbPackS* thisptr, const char* Key, uint64_t val);
	DLLEXPORT void PbPackS_SetVarInt(PbPackS* thisptr, const char* Key, uint64_t val);
	DLLEXPORT void PbPackS_SetZigZag32(PbPackS* thisptr, const char* Key, int32_t val);
	DLLEXPORT void PbPackS_SetZigZag64(PbPackS* thisptr, const char* Key, int64_t val);
	DLLEXPORT void PbPackS_Clear(PbPackS* thisptr);
	DLLEXPORT int PbPackS_Pack(PbPackS* thisptr);
	DLLEXPORT const char* PbPackS_GetData(PbPackS* thisptr);

	DLLEXPORT PbPackS* PbPackS_New();
	DLLEXPORT void PbPackS_Free(PbPackS* thisptr);

	DLLEXPORT BOOL PbUnPack_UnPack(PbUnPack* thisptr, const void* data, int size);
	DLLEXPORT void PbUnPack_GetVarInt(PbUnPack* thisptr, const char* path, uint64_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetVarIntArr(PbUnPack* thisptr, const char* path, vector<uint64_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetVarInt32(PbUnPack* thisptr, const char* path, uint32_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetVarInt32Arr(PbUnPack* thisptr, const char* path, vector<uint32_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetZigZag64(PbUnPack* thisptr, const char* path, int64_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetZigZag64Arr(PbUnPack* thisptr, const char* path, vector<int64_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetZigZag32(PbUnPack* thisptr, const char* path, int32_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetZigZag32Arr(PbUnPack* thisptr, const char* path, vector<int32_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetBin(PbUnPack* thisptr, const char* path, vector<char>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetBinArr(PbUnPack* thisptr, const char* path, vector<vector<char>>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetInt64(PbUnPack* thisptr, const char* path, uint64_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetInt64Arr(PbUnPack* thisptr, const char* path, vector<uint64_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetInt(PbUnPack* thisptr, const char* path, uint32_t* Val, int* IsOk);
	DLLEXPORT void PbUnPack_GetIntArr(PbUnPack* thisptr, const char* path, vector <uint32_t>* Val, int* IsOk);
	DLLEXPORT void PbUnPack_Clear(PbUnPack* thisptr);
	DLLEXPORT PbUnPack* PbUnPack_New();
	DLLEXPORT void PbUnPack_Free(PbUnPack* thisptr);

#ifdef __cplusplus
}
#endif
#endif