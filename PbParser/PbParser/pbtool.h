#ifndef __PBTOOL_H__
#define __PBTOOL_H__
#include <assert.h>
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <vector>






using namespace std;
#define WIRETYPE_VARINT 0 
#define WIRETYPE_FIXED64 1
#define WIRETYPE_LENGTH_DELIMITED 2
#define WIRETYPE_START_GROUP 3
#define WIRETYPE_END_GROUP 4
#define WIRETYPE_FIXED32 5


struct pbPath
{
	unsigned  short number = 0;
	unsigned  short type = 0;
	bool operator==(pbPath& t) {
		return t.number == this->number && this->type == t.type;
	};
};

void ParsKey(string& key, pbPath* path, BOOL* isArr, int* arrIndex);
//字节流转换为十六进制字符串
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);
string  BytesView(unsigned char* bytes, int len);
string UTF8ToGBK(char* str, int len);
string GBKToUTF8(const char* str);
string GBKToUTF8(const char* str, int len);
int IsTextUTF8(char* str, int length, BOOL* IsText);
string VarintArrView(vector<uint64_t> VarintArr);
vector<uint64_t> ReadVarintArr(unsigned char* data, int len);
int HexToBin(char* str, unsigned char* bytes);
vector<string> StringSplit(string dstr, string SplitStr);
int string_find(const std::string& str, const std::string& zstr, int pos);
vector<string> File_FindALL(const char* dirPath, string needName, BOOL FindSubDir);
string string_replace(const std::string& str, const std::string& zstr, const std::string& ztsr2 = "", size_t StartPos = 0, size_t replacecount = 0);
string string_GetMiddleStr(const std::string& str, const std::string& LeftStr, const std::string& RightStr);

#ifdef __cplusplus
extern "C" {
#endif



	 uint64_t ReadVarint(unsigned char* data, int len, int* readLen = NULL);
	 uint32_t ReadVarint32(unsigned char* data, int len, int* readLen = NULL);
	 int WriteVarint(uint64_t value, unsigned char* ptr);
	 uint32_t ZigZagEncode32(int32_t n);
	 int32_t ZigZagDecode32(uint32_t n);
	 uint64_t ZigZagEncode64(int64_t n);
	 int64_t ZigZagDecode64(uint64_t n);





#ifdef __cplusplus
}
#endif
#endif // !__PBTOOL_H__


