#ifndef  __PBPARSER__H
#define __PBPARSER__H
#include "pbtool.h"
#include "Pbio.h"
#include <map>


class PbUtilTreeNode;
typedef struct  UtilTreeNode
{
	pbPath path = {};	
	uint64_t val=0;
	vector<char> bytes;
	PbUtilTreeNode* Node=0;


};


class PbUtilTreeNode
{
public:
	PbUtilTreeNode();
	~PbUtilTreeNode();
	PbUtilTreeNode(const PbUtilTreeNode&) = delete;
	PbUtilTreeNode& operator=(const PbUtilTreeNode&) = delete;
	vector <UtilTreeNode> Data;
	void FreeALLNode();
private:

};

class pbUtil
{
public:
	pbUtil();
	~pbUtil();
	pbUtil(const pbUtil&) = delete;
	pbUtil& operator=(const pbUtil&) = delete;
	//获取打包代码
	const char* GetPackCode(char* data, int len);
private:

	BOOL MakePackCode(PbUtilTreeNode* rootNode, string RootKey);
	BOOL CheckNode(PbUtilTreeNode* rootNode);
	BOOL ParserBuf(char* data, int len);
	bool SkipField(PBIO_R* input, uint32_t tag, PbUtilTreeNode* root);
	BOOL SkipMessage(PBIO_R* input, PbUtilTreeNode* root);
	PbUtilTreeNode* m_RootTreeNode = 0;
	std::string m_CodeText;
public:

#define UseprotoFile 1
#if UseprotoFile


	BOOL protoFileToCode(const char* protoFileOrDir, const char* out_H_And_CPP_Dir, const char* out_H_And_CPP_name, const char* out_newProto_flieName=NULL, BOOL IsProto2 = 0,BOOL IsCheckGetVal=0);

	//const char* Get_protoFileToCode_Err() { return m_ParserProtoErr.c_str();};


private:

	typedef struct ProtoEmunInfo
	{
		string Name;
		string message;
	};
	enum msgArgsType 
	{
		//必须存在
		required=1,
		//可空
		optional=2,
		//可空数组
		repeated=3

	};
	typedef struct msgArgs
	{
		int Type = 0;
		string ArgType;
		string ArgName;
		int ArgId;
		string ArgDes;
		int _option1 = 0;

	};
	typedef struct Protomessageinfo
	{
		string pkgName;
		string Name;
		string message;
		string ParentName;
		vector<msgArgs> Args;



	};
	BOOL ParserProto(const char* protoFileOrDir, BOOL IsProto2 = 0);
	BOOL ParserProtoMsg(vector<string>& TextList);
	void _enum_Parser(vector<string>& TextList);
	vector<msgArgs> _message_Args_Parser(string message,string packname);
	BOOL __RepairMessage();
	BOOL __SortMessage();
	BOOL __Args_Parser(string msgstr, pbUtil::msgArgs& Args);

	void __Build_protoFile(const char* protoFile);
	void __Build_HFile_And_CPPFile(const char* out_H_And_CPP_Dir, const char* out_H_And_CPP_name);

	std::string m_ParserProtoErr;
	BOOL m_ParserProto2 = 0;
	BOOL m_IsCheckGetVal = 0;


	vector<Protomessageinfo> Proto_messageList;
	vector<ProtoEmunInfo> Proto_enumList;

	vector<Protomessageinfo> Proto_MapList;
#endif // protoFile
};

#endif // ! 