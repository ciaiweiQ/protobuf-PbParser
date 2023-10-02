#include "pch.h"
#include "pbtool.h"

string  BytesView(unsigned char* bytes, int len)
{
	string str = "{";
	for (int i = 0; i < len; i++)
	{
		if (i == len - 1)
		{
			str = str + to_string(bytes[i]);

		}
		else {
			str = str + to_string(bytes[i]) + ",";
		}

	}

	str = str + "}";
	return str;
}
string VarintArrView(vector<uint64_t> VarintArr) {

	string str = "{";
	for (int i = 0; i < VarintArr.size(); i++)
	{
		if (i == VarintArr.size() - 1)
		{
			str = str + to_string(VarintArr[i]);

		}
		else {
			str = str + to_string(VarintArr[i]) + ",";
		}
	}
	str = str + "}";
	return str;
}
void ParsKey(string& key, pbPath* path, BOOL* isArr, int* arrIndex)
{
	int __pos = key.find("[");
	if (__pos > -1)
	{
		int __pos2 = key.find("]");
		string _indexstr = string(key.c_str() + __pos + 1, __pos2 - __pos - 1);
		if (_indexstr.empty())
		{
			arrIndex[0] = 0;
		}
		else
		{
			arrIndex[0] = atoi(_indexstr.c_str());
		}
		key = string(key.c_str(), __pos);
		isArr[0] = 1;
	}
	unsigned char keyData[10] = { 0 };
	int len = HexToBin((char*)key.c_str(), keyData);
	int tag = ReadVarint(keyData, len);
	path[0].type = tag & 7;
	path[0].number = tag >> 3;

}

vector<string> StringSplit(string dstr, string SplitStr) {
	vector<string> retStrArr;
	if (SplitStr.empty() || dstr == "")
	{
		retStrArr.push_back(dstr);
		return retStrArr;
	}
	size_t start = 0;
	size_t index = dstr.find(SplitStr, 0);
	while (index != dstr.npos)
	{
		if (start != index)
			retStrArr.push_back(dstr.substr(start, index - start));
		start = index + SplitStr.size();
		index = dstr.find(SplitStr, start);
	}
	if (dstr.substr(start) != "")
	{
		retStrArr.push_back(dstr.substr(start));
	}
	return retStrArr;
}


vector<string> File_FindALL(const char* dirPath, string needName, BOOL FindSubDir)
{
	vector<string> ret;
	WIN32_FIND_DATA fd;
	int len = strlen(dirPath);
	if (!dirPath || len <= 0)return ret;
	string path = dirPath;
	if (dirPath[strlen(dirPath) - 1] != '\\')
	{
		path = path + "\\";
	}
	vector<string> needNameArr = StringSplit(needName, "|");
	for (int i = 0; i < needNameArr.size(); i++)
	{
		string Findpath = path + needNameArr[i];
		HANDLE hFindFile = FindFirstFileA(Findpath.c_str(), &fd);
		if (hFindFile == INVALID_HANDLE_VALUE)
		{
			::FindClose(hFindFile);
			return ret;
		}
		BOOL bFinish = FALSE;
		BOOL bUserReture = TRUE;
		BOOL bIsDirectory;
		while (!bFinish)
		{
			bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if (bIsDirectory
				&& (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0))
			{
				bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
				continue;
			}
			if (!bIsDirectory)
			{
				ret.push_back(path + fd.cFileName);
			}
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
		}
		::FindClose(hFindFile);
	}
	if (FindSubDir)
	{
		string Findpath = path + "*.*";
		HANDLE hFindFile = FindFirstFileA(Findpath.c_str(), &fd);
		if (hFindFile == INVALID_HANDLE_VALUE)
		{
			::FindClose(hFindFile);
			return ret;
		}
		BOOL bFinish = FALSE;
		BOOL bUserReture = TRUE;
		BOOL bIsDirectory;
		while (!bFinish)
		{
			bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if (bIsDirectory
				&& (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0))
			{
				bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
				continue;
			}
			if (bIsDirectory) //是子目录
			{
				vector<string> retdir = File_FindALL((path + fd.cFileName).c_str(), needName, FindSubDir);
				for (size_t jj = 0; jj < retdir.size(); jj++)
				{
					ret.push_back(retdir[jj]);
				}
			}
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
		}
		::FindClose(hFindFile);
	}
	return ret;
}

int string_find(const std::string& str, const std::string& zstr, int pos) {

	unsigned char* _ptr = (unsigned char*)str.c_str();
	int _strlen = str.size();
	unsigned char* _ptr2 = (unsigned char*)zstr.c_str();
	int _strlen2 = zstr.size();
	if (pos < 0)pos = 0;
	int _pos = pos;
	while (1)
	{
		if (_strlen - _pos < _strlen2)return -1;
		if (memcmp(_ptr + _pos, _ptr2, _strlen2) == 0)return _pos;
		_pos += _ptr[_pos] > 0x80 ? 2 : 1;
	}
	return -1;
}
string string_replace(const std::string& str, const std::string& zstr, const std::string& ztsr2, size_t StartPos, size_t replacecount)
{

	if (zstr.empty())
	{
		return str;
	}
	if (replacecount <= 0)replacecount = 0xffffffffu;
	if (StartPos < 0)StartPos = 0;
	string ret = str;
	string::size_type pos = StartPos;
	string::size_type a = zstr.size();
	string::size_type b = ztsr2.size();
	while ((pos = string_find(ret, zstr, pos)) != string::npos && replacecount > 0)
	{
		ret.replace(pos, a, ztsr2);
		pos += b;
		replacecount--;
	}
	return ret;
}

string string_GetMiddleStr(const std::string& str, const std::string& LeftStr, const std::string& RightStr)
{
	int _pos1 = string_find(str, LeftStr,0);
	if (_pos1 > -1)
	{
		_pos1 += LeftStr.size();
		int _pos2 = string_find(str, RightStr, _pos1);
		if (_pos2 > -1)
		{
			return	str.substr(_pos1, _pos2 - _pos1);
		}
	}
	return "";
}

//字节流转换为十六进制字符串
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
{
	short i;
	unsigned char highByte, lowByte;

	for (i = 0; i < sourceLen; i++)
	{
		highByte = source[i] >> 4;
		lowByte = source[i] & 0x0f;

		highByte += 0x30;

		if (highByte > 0x39)
			dest[i * 2] = highByte + 0x07;
		else
			dest[i * 2] = highByte;

		lowByte += 0x30;
		if (lowByte > 0x39)
			dest[i * 2 + 1] = lowByte + 0x07;
		else
			dest[i * 2 + 1] = lowByte;
	}
	return;
}
int HexToBin(char* str, unsigned char* bytes)
{
	int i = 0, retLen = 0;
	unsigned char  highByte, lowByte;
	while (str[i] != 0)
	{
		if (str[i] == ' ')
		{
			i++;
			continue;
		}
		highByte = toupper(str[i]);
		lowByte = toupper(str[i + 1]);

		if (highByte > 0x39)
			highByte -= 0x37;
		else
			highByte -= 0x30;

		if (lowByte > 0x39)
			lowByte -= 0x37;
		else
			lowByte -= 0x30;

		bytes[retLen++] = (highByte << 4) | lowByte;
		i += 2;
	}
	return retLen;
}

string UTF8ToGBK(char* str, int len)
{
	string result;
	WCHAR* strSrc;
	CHAR* szRes;
	char t = 0;

	char* ss = new char[len + 1];
	memcpy(ss, str, len);
	ss[len] = 0;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, ss, -1, nullptr, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, ss, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, nullptr, 0, nullptr, nullptr);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, nullptr, nullptr);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;
	delete[] ss;
	return result;
}

string GBKToUTF8(const char* str, int len)
{
	if (!str)return"";
	string result;
	WCHAR* strSrc;
	CHAR* szRes;

	char* _str = (char*)malloc(len + 1);
	memcpy(_str, str, len);
	_str[len] = 0;
	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, _str, -1, nullptr, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, _str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, nullptr, 0, nullptr, nullptr);
	szRes = new CHAR[i + 1];
	memset(szRes, 0, i + 1);
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, nullptr, nullptr);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;
	free(_str);

	return result;
}

string GBKToUTF8(const char* str)
{
	string result;
	WCHAR* strSrc;
	CHAR* szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, nullptr, 0, nullptr, nullptr);
	szRes = new CHAR[i + 1];
	memset(szRes, 0, i + 1);
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, nullptr, nullptr);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

int IsTextUTF8(char* str, int length, BOOL* IsText)
{
	int i;
	DWORD nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	UCHAR chr;
	BOOL bAllAscii = TRUE; //如果全部都是ASCII, 说明不是UTF-8
	BOOL IsUtf = 1;
	IsText[0] = 1;
	for (i = 0; i < length; i++)
	{
		chr = *(str + i);
		if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii = FALSE;
		if (chr == 0)
		{
			IsText[0] = 0;
			return 0;
		}
		if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else
				{
					IsUtf = 0;
					break;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if ((chr & 0xC0) != 0x80)
			{
				IsUtf = 0;
				break;
			}
			nBytes--;
		}
	}
	if (nBytes > 0) //违返规则
	{
		IsUtf = 0;
	}
	if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
	{
		IsUtf = 0;

	}


	if (!IsUtf)
	{
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] < 32 || str[i] >= 127)
			{
				IsText[0] = 0;
				return 0;
			}
		}
	}
	return IsUtf;
}
vector<uint64_t> ReadVarintArr(unsigned char* data, int len)
{
	vector<uint64_t> IntArr;
	IntArr.clear();

	unsigned char* buffer_ = data;
	if (data[len - 1] < 128)
	{
		while (buffer_ < data + len)
		{
			unsigned char b = 0;
			uint64_t result = 0;
			int count = 0;
			do {
				if (count >= 10)
				{
					IntArr.clear();
					return IntArr;
				}		
				b = *buffer_;
				result |= static_cast<uint64_t>(b & 0x7F) << (7 * count);
				buffer_++;
				++count;

			} while (b & 0x80);
			IntArr.push_back(result);
		}
	}
	return IntArr;
}











uint64_t ReadVarint(unsigned char* data, int len, int* readLen) {

	unsigned char b = 0;
	uint64_t result = 0;
	int count = 0;
	do {
		if (count >= len || count >= 10)//64位最大10 32位最大5
		{
			if (readLen)readLen[0] = 0;
			return 0;
		};
		b = *data;
		result |= static_cast<uint64_t>(b & 0x7F) << (7 * count);
		data++;
		++count;
	} while (b & 0x80);
	if (readLen)readLen[0] = count;

	return result;
}
uint32_t ReadVarint32(unsigned char* data, int len, int* readLen)
{
	unsigned char b = 0;
	uint32_t result = 0;
	int count = 0;
	do {
		if (count >= len || count >= 5)//64位最大10 32位最大5
		{
			if (readLen)readLen[0] = 0;
			return 0;
		};
		b = *data;
		result |= static_cast<uint32_t>(b & 0x7F) << (7 * count);
		data++;
		++count;
	} while (b & 0x80);
	if (readLen)readLen[0] = count;

	return result;
}
int WriteVarint(uint64_t value, unsigned char* ptr)
{
	ptr[0] = static_cast<unsigned char>(value);
	if (value < 0x80) {
		return 1;
	}
	ptr[0] |= 0x80;
	value >>= 7;
	ptr[1] = static_cast<unsigned char>(value);
	if (value < 0x80) {
		return 2;
	}
	unsigned char* ptr2 = ptr + 2;
	do {

		ptr2[-1] |= (0x80);
		value >>= 7;
		*ptr2 = static_cast<unsigned char>(value);
		++ptr2;
	} while (value >= 0x80);
	return ptr2 - ptr;

}

uint32_t ZigZagEncode32(int32_t n)
{
	return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
}

int32_t ZigZagDecode32(uint32_t n)
{
	return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
}

uint64_t ZigZagEncode64(int64_t n)
{
	return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
}

int64_t ZigZagDecode64(uint64_t n)
{
	return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
}

