#include "pch.h"
#include"pbUtil.h"


bool pbUtil::SkipField(PBIO_R* input, uint32_t tag, PbUtilTreeNode* root)
{

	int number = (tag >> 3);
	if (number == 0) return false;
	int WireType = (tag & 7);
	UtilTreeNode node = {};
	node.path.number = tag;
	node.path.type = WireType;
	pbPath _path;
	_path.number = tag;
	_path.type = WireType;




	switch (WireType)
	{
	case WIRETYPE_VARINT:
	{
		uint64_t value;
		if (!input->ReadVarint64(&value)) return false;
		node.val = value;
		root->Data.push_back(node);
		return true;
	}
	case WIRETYPE_FIXED64:
	{
		uint64_t value;
		if (!input->ReadInt64(&value)) return false;
		node.val = value;
		root->Data.push_back(node);
		return true;
	}
	case WIRETYPE_LENGTH_DELIMITED: {
		uint32_t length;
		if (!input->ReadVarint32(&length)) return false;
		BOOL IsRead = 0;
		string value = "";
		if (!input->ReadString(&value, length))return false;

		if (!value.empty())
		{
			PbUtilTreeNode* _root = new PbUtilTreeNode;
			PBIO_R _input((unsigned char*)value.c_str(), value.size());
			if (SkipMessage(&_input, _root) && _input.buffer_== _input.buffer_end_)
			{
				node.Node = _root;
				node.bytes.assign(value.c_str(), value.c_str() + value.size());
				root->Data.push_back(node);
			}
			else
			{
				
				node.bytes.assign(value.c_str(), value.c_str() + value.size());
				root->Data.push_back(node);
				delete _root;
			}
		}
		else
		{
			node.bytes.assign(value.c_str(), value.c_str() + value.size());
			root->Data.push_back(node);
		}
		return true;
	}
	case WIRETYPE_START_GROUP: {

		if (!SkipMessage(input, root)) {
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
		node.val = value;
		root->Data.push_back(node);
		return true;
	}
	default: {
		return false;
	}
	}
	return false;

}

BOOL pbUtil::SkipMessage(PBIO_R* input, PbUtilTreeNode* root)
{
	while (true)
	{
		uint32_t tag = input->ReadTag();
		if (tag == 0) 
		{
			if (input->buffer_== input->buffer_end_)
			{
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
		if (!SkipField(input, tag, root)) return false;
	}
	return false;
}



pbUtil::pbUtil()
{

}
pbUtil::~pbUtil()
{
	if (this->m_RootTreeNode)
	{
		this->m_RootTreeNode->FreeALLNode();
		delete this->m_RootTreeNode;
		this->m_RootTreeNode = 0;
	}

}
const char* pbUtil::GetPackCode(char* data, int len)
{
	if (this->ParserBuf(data, len))
	{
		if (this->m_RootTreeNode)
		{
			m_CodeText.clear();
			m_CodeText.append("PbPackS pb;\r\n");
			if (MakePackCode(this->m_RootTreeNode, ""))
			{
				return m_CodeText.c_str();
			}
		}
	}
	return 0;
}



BOOL pbUtil::MakePackCode(PbUtilTreeNode* rootNode, string RootKey)
{
	string KeyStr;
	string pbStr;
	int maxlen = rootNode->Data.size();
	int arrcount = 0;
	for (int i = 0; i < maxlen; i++)
	{	
		BOOL _isArr = 0;
		unsigned char bytes[10] = { 0 };
		char KeyStrBytes[48] = { 0 };
		long long Key = rootNode->Data[i].path.number | (rootNode->Data[i].path.type);
		int byteLen = WriteVarint(Key, bytes);
		ByteToHexStr(bytes, KeyStrBytes, byteLen);
		KeyStr = KeyStrBytes;
		unsigned long long val = rootNode->Data[i].val;
		if (!RootKey.empty())
		{
			KeyStr = RootKey + "." + KeyStr;
		}

		if (i + 1 < maxlen)
		{
			if (rootNode->Data[i + 1].path == rootNode->Data[i].path)
			{
				_isArr = 1;
				arrcount++;
			}
			else if (i != 0 && rootNode->Data[i - 1].path == rootNode->Data[i].path)
			{
				_isArr = 1;
				arrcount++;
			}
		}
		else if (i != 0 && rootNode->Data[i - 1].path == rootNode->Data[i].path)
		{
			_isArr = 1;
			arrcount++;
		}	
		if (_isArr)
		{
			KeyStr = KeyStr + "[" + to_string(arrcount - 1) + "]";
		}

		if (rootNode->Data[i].Node)
		{
			MakePackCode(rootNode->Data[i].Node, KeyStr);
		}
		else
		{
			switch (rootNode->Data[i].path.type)
			{
			case WIRETYPE_VARINT:
			{
				pbStr = "pb.SetVarInt (\"" + KeyStr + "\", " + to_string(val) + ");";
				break;
			}
			case WIRETYPE_FIXED32: {

				pbStr = "pb.SetInt (\"" + KeyStr + "\", " + to_string(val) + ");";
				break;
			}
			case WIRETYPE_FIXED64: {
				pbStr = "pb.SetLong (\"" + KeyStr + "\", " + to_string(val) + ");";
				break;
			}
			case WIRETYPE_LENGTH_DELIMITED:
			{
				vector< char> value = rootNode->Data[i].bytes;
				BOOL Istext;
				BOOL IsUtf = IsTextUTF8((char*)rootNode->Data[i].bytes.data(), rootNode->Data[i].bytes.size(), &Istext);
				if (IsUtf)
				{
					string utfstr = UTF8ToGBK((char*)value.data(), value.size());
					pbStr = "pb.SetUTF (\"" + KeyStr + "\", \"" + utfstr + "\");//" + BytesView((unsigned char*)value.data(), value.size());
				}
				else if (Istext) {
					pbStr = "pb.SetStr (\"" + KeyStr + "\", \"" + string(value.data(), value.size()) + "\");//" + BytesView((unsigned char*)value.data(), value.size());
				}
				else
				{
					vector<uint64_t> intArr = ReadVarintArr((unsigned char*)value.data(), value.size());
					if (!intArr.empty())
					{
						pbStr = "pb.SetVarIntArr (\"" + KeyStr + "\", " + VarintArrView(intArr) + ");// " + BytesView((unsigned char*)value.data(), value.size());
					}
					else
					{
						string _tmpname;
						int _c = 1;
						while (1)
						{
							_tmpname = "_tmp_"+to_string(_c);
							if ((int)m_CodeText.find( string("char ") + _tmpname + string("[]="))==-1)
							{
								break;
							}
							_c++;
						}

						pbStr = string("char ")+ _tmpname + string("[]=") + BytesView((unsigned char*)value.data(), value.size()) + ";\r\n";
						pbStr = pbStr + "pb.SetBin (\"" + KeyStr + "\", " + _tmpname + ",sizeof("+ _tmpname +"));";
					}
				}
				break;
			}
			default:
				break;

			}
			if (!pbStr.empty())
			{
				m_CodeText.append(pbStr);
				m_CodeText.append("\r\n");
			}
		}
	}
	return 1;
}

BOOL pbUtil::CheckNode(PbUtilTreeNode* rootNode)
{

	int maxlen = rootNode->Data.size();
	BOOL ret = 1;
	for (int i = 0; i < maxlen; i++)
	{
		if ( rootNode->Data[i].Node)
		{	
			BOOL IshaveNode = 1;
			for (int j = 0; j < maxlen; j++)
			{
				if (j != i && rootNode->Data[i].path == rootNode->Data[j].path)
				{
					if (!rootNode->Data[j].Node)
					{
						IshaveNode = 0;
						break;
					}
				}
			}
			if (!IshaveNode)
			{

				delete rootNode->Data[i].Node;
				rootNode->Data[i].Node = 0;
				ret = 0;
			}
		}
		if (rootNode->Data[i].Node)
		{
			if (!CheckNode(rootNode->Data[i].Node))
			{
				ret =0;
			}
		}
	}
	return ret;

}

BOOL pbUtil::ParserBuf(char* data, int len)
{
	PBIO_R input((unsigned char*)data, len);
	if (this->m_RootTreeNode)
	{
		this->m_RootTreeNode->FreeALLNode();
		delete this->m_RootTreeNode;
		this->m_RootTreeNode = 0;
	}
	this->m_RootTreeNode = new PbUtilTreeNode;
	BOOL ret = SkipMessage(&input, this->m_RootTreeNode);
	while (!CheckNode(this->m_RootTreeNode));


	return ret;
}
PbUtilTreeNode::PbUtilTreeNode()
{
	Data.clear();
}
PbUtilTreeNode::~PbUtilTreeNode()
{
	FreeALLNode();
}

void PbUtilTreeNode::FreeALLNode()
{
	for (int i = 0; i < this->Data.size(); i++)
	{
		if (this->Data[i].Node)
		{
			delete this->Data[i].Node;
			this->Data[i].Node = 0;
		}
	}
	this->Data.clear();
}



#if UseprotoFile
BOOL pbUtil::protoFileToCode(const char* protoFileOrDir, const char* out_H_And_CPP_Dir, const char* out_H_And_CPP_name, const char* out_newProto_flieName, BOOL IsProto2, BOOL IsCheckGetVal)
{
	m_ParserProtoErr.clear();
	m_IsCheckGetVal = IsCheckGetVal;
	if (!ParserProto(protoFileOrDir, IsProto2))return 0;
	//
	if (out_newProto_flieName)
	{
		__Build_protoFile(out_newProto_flieName);
	}
	
	__Build_HFile_And_CPPFile(out_H_And_CPP_Dir, out_H_And_CPP_name);
	return 1;

}

BOOL pbUtil::ParserProto(const char* protoFileOrDir, BOOL IsProto2)
{
	string protoFile = string(protoFileOrDir);
	vector<string> FileList;
	if ((int)protoFile.find(".proto") > -1)
	{
		FileList.push_back(protoFile);
	}
	else
	{
		FileList = File_FindALL(protoFileOrDir, "*.proto", 1);
	}

	if (FileList.size() == 0)
	{
		m_ParserProtoErr.append("没找到.proto文件\r\n");
		return 0;
	}


	vector<string> TextList;
	TextList.reserve(TextList.size());
	for (int i = 0; i < FileList.size(); i++)
	{

		FILE* file;
		file = fopen(FileList[i].c_str(), "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			int length = ftell(file);
			fseek(file, 0, SEEK_SET);
			char* data = (char*)malloc(length);
			fread(data, 1, length, file);
			BOOL _IsText;
			string _str;
			if (IsTextUTF8(data, length, &_IsText))
			{
				_str = UTF8ToGBK(data, length);
			}
			else
			{
				_str = string(data, length);
			}
			free(data);
			fclose(file);
			_str = string_replace(_str, "\t", " ");
			_str = string_replace(_str,"  "," ");
			TextList.push_back(_str);
		}
	}
	BOOL _isProto2 = 0;
	BOOL _isProto3 = 0;
	for (int i = 0; i < TextList.size(); i++)
	{
		if ((int)TextList[i].find("\"proto2\"") > -1)
		{
			_isProto2 = 1;
		}
		if ((int)TextList[i].find("\"proto3\"") > -1)
		{
			_isProto3 = 1;
		}
	}
	if (_isProto3 && _isProto2)
	{
		m_ParserProtoErr.append("同时存在proto2语法和proto3语法");
		return 0;
	}
	if (!_isProto3 && !_isProto2)
	{
		m_ParserProto2 = IsProto2;
	}
	else
	{
		m_ParserProto2 = _isProto2;
	}
	return ParserProtoMsg(TextList);
}

BOOL pbUtil::ParserProtoMsg(vector<string>& TextList)
{
	Proto_enumList.clear();
	Proto_messageList.clear();
	_enum_Parser(TextList);
	//提取message
	for (int i = 0; i < TextList.size(); i++)
	{
		string _msg = TextList[i];
		string pkgName = "";
		if ((int)_msg.find("package ")>-1)
		{
			pkgName = string_GetMiddleStr(_msg, "package ", ";");
			pkgName = string_replace(pkgName," ");
		}
		const char* _msgptr = _msg.c_str();
		while (1)
		{
			int _pos = _msg.find("message ");
			if (_pos > -1)
			{
				int _lpos = 0;
				int _L_count = 0;
				int _R_count = 0;
				for (int j = _pos; j < _msg.size(); j++)
				{
					if (_msgptr[j] == '{')_L_count++;
					if (_msgptr[j] == '}')_R_count++;
					if (_L_count > 0 && _R_count > 0 && _L_count == _R_count)
					{
						_lpos = j;
						break;
					}
					if (_L_count< _R_count)
					{
						_lpos =0;
						break;
					}
				}
				if (_lpos > 0)
				{
					string _message = string(_msgptr + _pos, _lpos - _pos + 1);

					Protomessageinfo _info;
					_info.message = _message;
					_info.Name = string_GetMiddleStr(_message, "message ", "{");
					_info.Name = string_replace(_info.Name, "\r");
					_info.Name = string_replace(_info.Name, "\n");
					_info.Name = string_replace(_info.Name, " ");
					_info.Name = string_replace(_info.Name, "\t");
					_info.message = _message;
					_info.pkgName = pkgName;
	
					_info.Args = _message_Args_Parser(_message, pkgName);
					Proto_messageList.push_back(_info);
					_msg = string(_msgptr + _lpos + 2);
					_msgptr = _msg.c_str();

				}
				else if(_L_count < _R_count)
				{
					_msg = string(_msgptr + _pos + 9);
					_msgptr = _msg.c_str();
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	//提取嵌套message
	BOOL _isHavemsg = 1;
	while (_isHavemsg)
	{
		_isHavemsg = 0;
		for (int i = 0; i < Proto_messageList.size(); i++)
		{

			string _msg = Proto_messageList[i].message;
			const char* _msgptr = _msg.c_str();
			BOOL _isfindmsg = 0;
			while (1)
			{
				int _pos = string_find(_msg, "message ", 8);
				if (_pos > -1)
				{
					int _lpos = 0;
					int _L_count = 0;
					int _R_count = 0;
					for (int j = _pos; j < _msg.size(); j++)
					{
						if (_msgptr[j] == '{')_L_count++;
						if (_msgptr[j] == '}')_R_count++;
						if (_L_count > 0 && _R_count > 0 && _L_count == _R_count)
						{
							_lpos = j;
							break;
						}
						if (_L_count < _R_count) {
							_lpos = 0;
							break;
						}
					}
					if (_lpos > 0)
					{
						_isHavemsg = 1;
						_isfindmsg = 1;
						string _message = string(_msgptr + _pos, _lpos - _pos + 1);

						Protomessageinfo _info;
						_info.message = _message;
						_info.Name = string_GetMiddleStr(_message, "message ", "{");
						_info.Name = string_replace(_info.Name, "\r");
						_info.Name = string_replace(_info.Name, "\n");
						_info.Name = string_replace(_info.Name, " ");
						_info.Name = string_replace(_info.Name, "\t");
						_info.pkgName = Proto_messageList[i].pkgName;
						_info.ParentName = Proto_messageList[i].Name;
						_info.message = _message;
						_info.Args = _message_Args_Parser(_message, _info.pkgName);
						Proto_messageList.push_back(_info);
						Proto_messageList[i].message = string_replace(Proto_messageList[i].message, _message);
						_msg = string(_msgptr + _lpos + 2);
						_msgptr = _msg.c_str();
					}
					else if (_L_count < _R_count)
					{
						_msg = string(_msgptr + _pos + 9);
						_msgptr = _msg.c_str();
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			if (_isfindmsg)
			{
				break;
			}
		}

	}
	Proto_MapList.clear();
	//修复message的类型
	__RepairMessage();
	for (int i = 0; i < Proto_MapList.size(); i++)
	{
		Proto_messageList.push_back( Proto_MapList[i]);
	}
	//排序_去重 message
	__SortMessage();


	
	return 1;
}

void pbUtil::_enum_Parser(vector<string>& TextList)
{
	//先将所有的 enum 提取出来 并清除所有的enum
	for (int i = 0; i < TextList.size(); i++)
	{
		string _msg = TextList[i];
		const char* _msgptr = _msg.c_str();
		while (1)
		{
			int _pos = _msg.find("enum ");
			if (_pos > -1)
			{
				int _lpos = 0;
				int _L_count = 0;
				int _R_count = 0;
				for (int j = _pos; j < _msg.size(); j++)
				{
					if (_msgptr[j] == '{')_L_count++;
					if (_msgptr[j] == '}')_R_count++;
					if (_L_count > 0 && _R_count > 0 && _L_count == _R_count)
					{
						_lpos = j;
						break;
					}
					if (_L_count< _R_count)
					{
						_lpos = 0;
						break;
					}
				}
		
				if (_lpos > 0)
				{
					string _message = string(_msgptr + _pos, _lpos - _pos + 1);
					ProtoEmunInfo _info;
					_info.Name = string_GetMiddleStr(_message, "enum ", "{");
					_info.Name = string_replace(_info.Name, "\r");
					_info.Name = string_replace(_info.Name, "\n");
					_info.Name = string_replace(_info.Name, " ");
					_info.Name = string_replace(_info.Name, "\t");
					_info.message = _message;
					Proto_enumList.push_back(_info);


					TextList[i] = string_replace(TextList[i], _message);
					_msg = string(_msgptr + _lpos + 2);
					_msgptr = _msg.c_str();
				}
				else if (_L_count < _R_count)
				{
					_msg = string(_msgptr + _pos + 5);
					_msgptr = _msg.c_str();
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

}

vector<pbUtil::msgArgs> pbUtil::_message_Args_Parser(string message, string packname)
{
	vector<pbUtil::msgArgs> ret;
	string _message_ = message;
	string _msg = _message_;
	const char* _msgptr = _msg.c_str();
	BOOL _isfindmsg = 0;
	while (1)
	{
		int _pos = string_find(_msg, "message ", 8);
		if (_pos > -1)
		{
			int _lpos = 0;
			int _L_count = 0;
			int _R_count = 0;
			for (int j = _pos; j < _msg.size(); j++)
			{
				if (_msgptr[j] == '{')_L_count++;
				if (_msgptr[j] == '}')_R_count++;
				if (_L_count > 0 && _R_count > 0 && _L_count == _R_count)
				{
					_lpos = j;
					break;
				}
				if (_L_count< _R_count)
				{
					_lpos = 0;
					break;
				}
			}
			if (_lpos > 0)
			{

				_isfindmsg = 1;
				string _message = string(_msgptr + _pos, _lpos - _pos + 1);
				_message_ = string_replace(_message_, _message);

				_msg = string(_msgptr + _lpos + 2);		
				_msgptr = _msg.c_str();			
				
			}
			else if(_L_count < _R_count)
			{
				_msg = string(_msgptr + _pos + 8);
				_msgptr = _msg.c_str();
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}



	_message_ = string_replace(_message_, "\r");
	_message_ = string_replace(_message_, "\t", " ");
	vector<string> msgList = StringSplit(_message_, "\n");
	int _MsgId = 1;

	for (int i = 0; i < msgList.size(); i++)
	{
		pbUtil::msgArgs Args = {};

		string msgstr = msgList[i];
		while (msgstr.size() > 0 && msgstr[0] == ' ')
		{
			msgstr = string(msgstr.c_str() + 1);
		}
		if ((int)msgstr.find(", ") > -1)//map<key, val>
		{
			msgstr = string_replace(msgstr, ", ", ",");
		}

		string _idstr = string_GetMiddleStr(msgstr, "=", ";");
		_idstr = string_replace(_idstr, " ");
		int __msgid = atoi(_idstr.c_str());


		int despos = (int)msgstr.find("//");
		string _des = "";
		if (despos > -1)
		{
			_des = string(msgstr.c_str() + despos + 2, msgstr.size() - (despos + 2));
		}
		if ((int)msgstr.find("required ") > -1)
		{
			Args.ArgType = string_GetMiddleStr(msgstr, "required ", " ");
			Args.Type = msgArgsType::required;
			Args.ArgDes = _des;
			if (!__Args_Parser(msgstr, Args))
			{
				assert(!"解析失败");
			}
			if (__msgid != 0)
			{
				_MsgId = __msgid;
			}

			Args.ArgId = _MsgId;
			_MsgId++;


		}
		else if ((int)msgstr.find("optional ") > -1)
		{
			Args.ArgType = string_GetMiddleStr(msgstr, "optional ", " ");
			Args.Type = msgArgsType::optional;
			Args.ArgDes = _des;
			if (!__Args_Parser(msgstr, Args))
			{
				assert(!"解析失败");
			}
			if (__msgid != 0)
			{
				_MsgId = __msgid;
			}

			Args.ArgId = _MsgId;
			_MsgId++;

		}
		else if ((int)msgstr.find("repeated ") > -1)
		{
			Args.ArgType = string_GetMiddleStr(msgstr, "repeated ", " ");
			Args.Type = msgArgsType::repeated;
			Args.ArgDes = _des;
			if (!__Args_Parser(msgstr, Args))
			{
				assert(!"解析失败");
			}
			if (__msgid != 0)
			{
				_MsgId = __msgid;
			}

			Args.ArgId = _MsgId;
			_MsgId++;

		}
		else if ((int)msgstr.find("message ") == -1)
		{
			int _pos = (int)msgstr.find(" ");
			if (_pos > -1)
			{
				Args.ArgType = string(msgstr.c_str(), _pos);

				Args.Type = msgArgsType::optional;
				Args.ArgDes = _des;
				if (__Args_Parser(msgstr, Args))
				{
					if (__msgid != 0)
					{
						_MsgId = __msgid;
					}

					Args.ArgId = _MsgId;
					_MsgId++;
				}
			}
		}
		Args.ArgType = string_replace(Args.ArgType, " ");
		//if ((int)Args.ArgType.find("map<") == 0 && (int)Args.ArgType.find(">") > -1)
		//{
		//	//map<key,val>
		//	string _tmpstr = string_GetMiddleStr(Args.ArgType, "map<", ">");
		//	vector<string> _tmpstrarr = StringSplit(Args.ArgType, ",");
		//	if (_tmpstrarr.size() == 2)
		//	{




		//	}
		//}




		if (Args.ArgId && Args.ArgType != "" && Args.ArgName != "")
		{
			ret.push_back(Args);
		}
	}
	return ret;
}

BOOL pbUtil::__RepairMessage()
{
	map<string, vector<Protomessageinfo*>> _map;
	for (int i = 0; i < Proto_messageList.size(); i++)
	{	
		_map[Proto_messageList[i].pkgName].push_back(&Proto_messageList[i]);
	}
	auto it= _map.begin();
	while (it!= _map.end())
	{
		for (int i = 0; i < it->second.size(); i++)
		{		
			for (int j = 0; j < it->second[i]->Args.size(); j++)
			{				
			
				int _map_pos = it->second[i]->Args[j].ArgType.find("map<");
				if (_map_pos>-1)
				{
					//修复map类型
					string _tmpstr = string_GetMiddleStr(it->second[i]->Args[j].ArgType, "map<", ">");
					vector<string> _tmpstrarr = StringSplit(_tmpstr, ",");
					if (_tmpstrarr.size() == 2)
					{
						string _type1="";
						string _type2="";
						for (int _t = 0; _t < _tmpstrarr.size(); _t++)
						{

							for (int k = 0; k < it->second.size(); k++)
							{
								if (_tmpstrarr[_t] == it->second[k]->Name)
								{
									string _fname = "";
									if (it->second[k]->pkgName != "")
									{
										_fname = it->second[k]->pkgName;
									}
									if (it->second[k]->ParentName != "")
									{
										if (_fname == "")
										{
											_fname = it->second[k]->ParentName;
										}
										else
										{
											_fname = _fname + "_" + it->second[k]->ParentName;
										}
									}
									if (_fname == "")
									{
										_fname = _tmpstrarr[_t];
									}
									else
									{
										_fname = _fname + "_" + _tmpstrarr[_t];
									}
									_fname=  string_replace(_fname, ".", "_");
									_tmpstrarr[_t] = _fname;
								}

							}
						}

						_type1 = _tmpstrarr[0];
						_type2 = _tmpstrarr[1];

						Protomessageinfo _info;
						_info.Name = "map__" + _type1 + "__" + _type2;
						msgArgs Arg;
						Arg.ArgType = _type1;
						Arg.ArgId = 1;
						Arg.ArgName = "key";
						Arg.Type = msgArgsType::optional;
						_info.Args.push_back(Arg);
						Arg.ArgType = _type2;
						Arg.ArgId = 2;
						Arg.ArgName = "Val";
						Arg.Type = msgArgsType::optional;
						_info.Args.push_back(Arg);
						BOOL _isfind_map = 0;
						for (int __m = 0; __m < Proto_MapList.size(); __m++)
						{
							if (Proto_MapList[__m].Name== _info.Name)
							{
								_isfind_map = 1;
								break;
							}
						}
						if (!_isfind_map)
						{
							Proto_MapList.push_back(_info);
						}
						it->second[i]->Args[j].Type = msgArgsType::repeated;
						it->second[i]->Args[j].ArgType = _info.Name;
					}
				}
				else
				{
					BOOL _isenum=0;
					for (int _enum = 0; _enum < Proto_enumList.size(); _enum++)
					{
						if (Proto_enumList[_enum].Name== it->second[i]->Args[j].ArgType)
						{
							_isenum = 1;
							break;
						}
					}
					if (_isenum)
					{
						it->second[i]->Args[j].ArgType = "enum";
					}
					else
					{
						for (int k = 0; k < it->second.size(); k++)
						{
							if (it->second[i]->Args[j].ArgType == it->second[k]->Name)
							{
								string _fname = "";
								if (it->second[k]->pkgName != "")
								{
									_fname = it->second[k]->pkgName;
								}
								if (it->second[k]->ParentName != "")
								{
									if (_fname == "")
									{
										_fname = it->second[k]->ParentName;
									}
									else
									{
										_fname = _fname + "_" + it->second[k]->ParentName;
									}
								}
								if (_fname == "")
								{
									_fname = it->second[i]->Args[j].ArgType;
								}
								else
								{
									_fname = _fname + "_" + it->second[i]->Args[j].ArgType;
								}
								if (_fname=="CountPackage.Counts")
								{
									int dd = 1;
								}
								_fname = string_replace(_fname, ".", "_");
								it->second[i]->Args[j].ArgType = _fname;
							}

						}


					}
					
			
				}
						
			}		
		}	
		it++;
	}
	 it = _map.begin();
	while (it != _map.end()) 
	{
		for (int i = 0; i < it->second.size(); i++) {

			string _name = "";
			if (it->second[i]->pkgName != "")
			{
				_name = it->second[i]->pkgName;
			}
			if (it->second[i]->ParentName != "")
			{
				if (_name == "")
				{
					_name = it->second[i]->ParentName;
				}
				else
				{
					_name = _name + "_" + it->second[i]->ParentName;
				}
			}
			if (_name == "")
			{
				_name = it->second[i]->Name;
			}
			else
			{
				_name = _name + "_" + it->second[i]->Name;
			}
			_name = string_replace(_name, ".", "_");
			it->second[i]->Name = _name;

			for (int j = 0; j < it->second[i]->Args.size(); j++)
			{
				it->second[i]->Args[j].ArgType= string_replace(it->second[i]->Args[j].ArgType, ".", "_");
			}
		}



		it++;
	}
	return 1;
}

BOOL pbUtil::__SortMessage()
{

	int _s_count = 0;
	while (true)
	{
		//去重复
		while (true)
		{
			BOOL _ISDEL = 0;
			for (int i = 0; i < Proto_messageList.size(); i++)
			{
				for (int j = 0; j < Proto_messageList.size(); j++)
				{
					if (i!=j && Proto_messageList[i].Name== Proto_messageList[j].Name)
					{
						_ISDEL = 1;
						break;
					}
				}
				if (_ISDEL)
				{
					Proto_messageList.erase(Proto_messageList.begin() + i);
					break;
				}
			}
			if (!_ISDEL)
			{
				break;
			}
		}



		int _s_index = -1;
		BOOL IsS = 0;
		for (int i = 0; i < Proto_messageList.size(); i++)
		{
			string _name = Proto_messageList[i].Name;
			_s_index = -1;

			for (int j = 0; j < Proto_messageList.size(); j++)
			{
				if (j < i)
				{
					BOOL _isfind=0;
					for (int k = 0; k < Proto_messageList[j].Args.size(); k++)
					{
						if (Proto_messageList[j].Args[k].ArgType== _name)
						{
							_isfind = 1;
							break;
						}
					}
					if (_isfind)
					{
						_s_index = j;
					}
				}
			}
			if (_s_index>-1)
			{
				std::swap(Proto_messageList[i], Proto_messageList[_s_index]);
				IsS = 1;
				_s_count++;
			}
		}
		if (!IsS || _s_count>= Proto_messageList.size()*10)
		{
			break;
		}
	}
	return 1;
}

BOOL pbUtil::__Args_Parser(string msgstr, pbUtil::msgArgs& Args)
{
	Args.ArgName = string_GetMiddleStr(msgstr, Args.ArgType + " ", " ");
	if (Args.ArgName == "")
	{
		Args.ArgName = string_GetMiddleStr(msgstr, Args.ArgType + " ", "=");
	}
	int pos = (int)Args.ArgName.find("=");
	if (pos > -1)
	{
		Args.ArgName = string(Args.ArgName.c_str(), pos);
	}
	Args.ArgName = string_replace(Args.ArgName, " ");

	if (Args.ArgName != "")
	{
		return 1;
	}

	return 0;
}

void pbUtil::__Build_protoFile(const char* protoFile)
{
	string proto;
	proto.clear();
	for (int i = 0; i < Proto_messageList.size(); i++)
	{

		proto.append("message ");

		proto.append(Proto_messageList[i].Name);
		proto.append(" {\r\n");
		for (int j = 0; j < Proto_messageList[i].Args.size(); j++)
		{
			switch (Proto_messageList[i].Args[j].Type)
			{
			case msgArgsType::optional:
				proto.append(" optional ");
				break;
			case msgArgsType::repeated:
				proto.append(" repeated ");
				break;
			case msgArgsType::required:
				proto.append(" required ");
				break;
			default:
				assert(0);
				break;
			}
			proto.append(Proto_messageList[i].Args[j].ArgType);
			proto.append(" ");
			proto.append(Proto_messageList[i].Args[j].ArgName);
			proto.append(" = ");
			proto.append(to_string(Proto_messageList[i].Args[j].ArgId));
			proto.append(";");
			if (Proto_messageList[i].Args[j].ArgDes!="")
			{
				proto.append("//");
				proto.append(Proto_messageList[i].Args[j].ArgDes);

			}
			proto.append("\r\n");
		}


		proto.append(" }\r\n");
	}
	FILE* fp;
	fp = fopen(protoFile, "wb");
	if (fp)
	{
		fwrite(proto.c_str(), 1, proto.size(), fp);
		fclose(fp);
	}

}

void pbUtil::__Build_HFile_And_CPPFile(const char* out_H_And_CPP_Dir, const char* out_H_And_CPP_name)
{
	string H_Code;
	H_Code.append("#pragma once\r\n#include \"PbPackS.h\"\r\n#include \"PbUnPack.h\"\r\n#define USER_TOJSON 1\r\ntemplate<typename ... Args>\r\ninline string string_Format(const char* format, Args ... args)\r\n{\r\n	size_t size = snprintf(nullptr, 0, format, args ...) + 1;\r\n	char* buf = (char*)malloc(size);\r\n	snprintf(buf, size, format, args ...);\r\n	string _ret(buf, size - 1);\r\n	free(buf);\r\n	return _ret;\r\n}\r\ninline vector<char> pbobj_PackEx(const char* data, int len, string pbobjName) {\r\n\r\n	return vector<char>(data, data + len);\r\n}\r\nclass pbobj\r\n{\r\npublic:\r\n	pbobj(const pbobj& pb)\r\n	{\r\n		if (m_unpb)delete m_unpb;\r\n		if (m_pb)delete m_pb;\r\n		m_unpb = 0;\r\n		m_pb = 0;\r\n	};\r\n	pbobj() {\r\n\r\n	}\r\n	pbobj& operator=(const pbobj& s) {\r\n		if (this != &s)\r\n		{\r\n			if (m_unpb)delete m_unpb;\r\n			if (m_pb)delete m_pb;\r\n			m_unpb = 0;\r\n			m_pb = 0;\r\n		}\r\n		return *this;\r\n	}\r\n	virtual ~pbobj()\r\n	{\r\n		if (m_unpb)delete m_unpb;\r\n		if (m_pb)delete m_pb;\r\n	};\r\n	virtual void Parser(const void* data, int len) = 0;\r\n	virtual const char* Pack(int* retlen) = 0;\r\n	virtual vector<char> PackEx() = 0;\r\n\r\n#if USER_TOJSON\r\n	virtual string ToJson() = 0;\r\n#endif // USER_TOJSON\r\nprotected:\r\n\r\n	PbUnPack* m_unpb = 0;\r\n	PbPackS* m_pb = 0;\r\n};\r\n\r\n");

	string CPP_Code;
	{
	CPP_Code.append("#include \"pch.h\"\r\n");
	CPP_Code.append("#include \"" + string(out_H_And_CPP_name) + ".h\"\r\n");
	CPP_Code.append("\r\n#if USER_TOJSON\r\ntemplate<typename Arg>\r\ninline string pbobj_vector_tostring(vector<Arg>& val)\r\n{\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + val[i].ToJson();\r\n		}\r\n		else\r\n		{\r\n			str = str + val[i].ToJson() + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\n\r\ninline int pbstring_find(const std::string& str, const std::string& zstr, int pos) {\r\n\r\n	unsigned char* _ptr = (unsigned char*)str.c_str();\r\n	int _strlen = str.size();\r\n	unsigned char* _ptr2 = (unsigned char*)zstr.c_str();\r\n	int _strlen2 = zstr.size();\r\n	if (pos < 0)pos = 0;\r\n	int _pos = pos;\r\n	while (1)\r\n	{\r\n		if (_strlen - _pos < _strlen2)return -1;\r\n		if (memcmp(_ptr + _pos, _ptr2, _strlen2) == 0)return _pos;\r\n		_pos += _ptr[_pos] > 0x80 ? 2 : 1;\r\n	}\r\n	return -1;\r\n}\r\n\r\ninline string pbstring_replace(const std::string& str, const std::string& zstr, const std::string& ztsr2, size_t StartPos, size_t replacecount)\r\n{\r\n\r\n	if (zstr.empty())\r\n	{\r\n		return str;\r\n	}\r\n	if (replacecount <= 0)replacecount = 0xffffffffu;\r\n	if (StartPos < 0)StartPos = 0;\r\n	string ret = str;\r\n	string::size_type pos = StartPos;\r\n	string::size_type a = zstr.size();\r\n	string::size_type b = ztsr2.size();\r\n	while ((pos = pbstring_find(ret, zstr, pos)) != string::npos && replacecount > 0)\r\n	{\r\n		ret.replace(pos, a, ztsr2);\r\n		pos += b;\r\n		replacecount--;\r\n	}\r\n	return ret;\r\n}\r\n\r\ninline string pb_vector_tostring(vector<uint64_t> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<long long> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\n\r\ninline string pb_string_tojsonstring(string val) {\r\n	string str;\r\n	str = pbstring_replace(val,\"\\\"\",\"\\\\\\\"\",0,0);\r\n	str = pbstring_replace(str, \"\\r\", \"\\\\r\", 0, 0);\r\n	str = pbstring_replace(str, \"\\n\", \"\\\\n\", 0, 0);\r\n	str = pbstring_replace(str, \"\\t\", \"\\\\t\", 0, 0);\r\n	return str;\r\n}\r\n\r\ninline string pb_vector_tostring(vector<int> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<uint32_t> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<char> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<vector<char>> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + pb_vector_tostring(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + pb_vector_tostring(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<string> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + \"\\\"\" + pbstring_replace(val[i],\"\\\"\",\"\\\\\\\"\",0,0) + \"\\\"\";\r\n		}\r\n		else\r\n		{\r\n			str = str + \"\\\"\" + pbstring_replace(val[i], \"\\\"\", \"\\\\\\\"\", 0, 0) + \"\\\"\" + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<float> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\ninline string pb_vector_tostring(vector<double> val) {\r\n\r\n	string str = \"[\";\r\n	for (size_t i = 0; i < val.size(); i++)\r\n	{\r\n		if (i == val.size() - 1)\r\n		{\r\n			str = str + to_string(val[i]);\r\n		}\r\n		else\r\n		{\r\n			str = str + to_string(val[i]) + \",\";\r\n		}\r\n	}\r\n	str = str + \"]\";\r\n	return str;\r\n}\r\n#endif // USER_TOJSON\r\n");
	}
	static string __C__ = "|asm|auto|bool|break|case|catch|char|class|const|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|";
	for (int i = 0; i < Proto_messageList.size(); i++)
	{
		if (Proto_messageList[i].Name != "")
		{
			string C_member = "";
			for (int j = 0; j < Proto_messageList[i].Args.size(); j++)
			{
				int _pos = __C__.find("|" + Proto_messageList[i].Args[j].ArgName + "|");
				Proto_messageList[i].Args[j]._option1 = 0;
				if (_pos > -1)
				{
					Proto_messageList[i].Args[j].ArgName = Proto_messageList[i].Args[j].ArgName + "_";
					Proto_messageList[i].Args[j]._option1 = 1;
				}

				string _C_Type = "";
				BOOL _IS_NEED_INIT = 0;
				if (Proto_messageList[i].Args[j].ArgType == "sint64")
				{
					_C_Type = "long long";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "sint32")
				{
					_C_Type = "int";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "sfixed64")
				{
					_C_Type = "long long";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "sfixed32")
				{
					_C_Type = "int";
					_IS_NEED_INIT = 1;
				}

				else if (Proto_messageList[i].Args[j].ArgType == "enum")
				{
					_C_Type = "int";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "uint32")
				{
					_C_Type = "uint32_t";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "bytes")
				{
					_C_Type = "vector<char>";

				}
				else if (Proto_messageList[i].Args[j].ArgType == "bool")
				{
					_C_Type = "BOOL";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "fixed32")
				{
					_C_Type = "uint32_t";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "fixed64")
				{
					_C_Type = "uint64_t";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "int32")
				{
					_C_Type = "int";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "uint64")
				{
					_C_Type = "uint64_t";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "int64")
				{
					_C_Type = "long long";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "float")
				{
					_C_Type = "float";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "double")
				{
					_C_Type = "double";
					_IS_NEED_INIT = 1;
				}
				else if (Proto_messageList[i].Args[j].ArgType == "string")
				{
					_C_Type = "string";
					//_IS_NEED_INIT = 1;
				}
				else
				{
					_C_Type = Proto_messageList[i].Args[j].ArgType;
				}

				if (Proto_messageList[i].Args[j].Type == msgArgsType::repeated)
				{
					_IS_NEED_INIT = 0;
					_C_Type = "\tvector<" + _C_Type + ">";
				}
				else
				{
					_C_Type = "\t" + _C_Type;
				}


				if (_IS_NEED_INIT)
				{
					C_member = C_member + _C_Type + " " + Proto_messageList[i].Args[j].ArgName + " = 0;";
				}
				else
				{
					C_member = C_member + _C_Type + " " + Proto_messageList[i].Args[j].ArgName + ";";
				}
				if (Proto_messageList[i].Args[j].ArgDes != "")
				{
					C_member = C_member + "//" + Proto_messageList[i].Args[j].ArgDes;
				}
				C_member = C_member + "\r\n";
				if (m_IsCheckGetVal)
				{
					C_member = C_member + "\tBOOL IsOk_" + Proto_messageList[i].Args[j].ArgName + " = 0;\r\n";
				}

			}

			string _Json_code = "";
			string _Json_arg_code = "";
			string _Unpack_code = "";
			string _Pack_code = "";

			int _bin_count = 0;
			int _binArr_count = 0;
			typedef struct _Build_tmpinfo
			{
				string SaveName;//保存变量名
				BOOL IsArr;
				string tmpType;//解析类型
				string tmpArgName;//解析变量名
			};
			vector<_Build_tmpinfo> BuildtmpinfoList;
			BuildtmpinfoList.clear();
			BOOL IsWPacKLen = 0;
			BOOL IsWPacKArrLen = 0;
			for (int j = 0; j < Proto_messageList[i].Args.size(); j++)
			{
				string CheckArgstr = "";
				if (m_IsCheckGetVal)
				{
					CheckArgstr = "&IsOk_" + Proto_messageList[i].Args[j].ArgName;
				}
				else
				{
					CheckArgstr = "NULL";
				}
				string Json_argName = "";
				if (Proto_messageList[i].Args[j]._option1)
				{
					Json_argName = string(Proto_messageList[i].Args[j].ArgName.c_str(), Proto_messageList[i].Args[j].ArgName.size() - 1);
				}
				else
				{
					Json_argName = Proto_messageList[i].Args[j].ArgName;
				}
				string _argName = Proto_messageList[i].Args[j].ArgName;
				string _argType = Proto_messageList[i].Args[j].ArgType;
				int msgid = Proto_messageList[i].Args[j].ArgId;
				int type = Proto_messageList[i].Args[j].Type;
				char keybyte[10] = { 0 };
				char keyStrbyte[21] = { 0 };
				if (_argType == "int32" || _argType == "uint32" || _argType == "enum" || _argType == "bool")
				{


					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 0;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetVarInt32 (\"" + StrKey + "\", (uint32_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetVarInt(\"" + StrKey + "\", (uint64_t)this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						if (_argType == "bool")
						{
							_Json_arg_code.append("this->" + _argName + "?\"true\":\"false\",");
						}
						else
						{
							_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");
						}
					}
					else if (type == msgArgsType::repeated)
					{
						int wtype;
						if (!m_ParserProto2)
						{
							wtype = 2;
						}
						else
						{
							wtype = 0;
						}
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetVarInt32Arr (\"" + StrKey + "\", (vector<uint32_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetVarInt32Arr(\"" + StrKey + "\", *(vector<uint32_t>*)&this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}



				}
				else if (_argType == "sint32")
				{

					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 0;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetZigZag32 (\"" + StrKey + "\", (int32_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetZigZag32(\"" + StrKey + "\", (int32_t)&this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						int wtype;
						if (!m_ParserProto2)
						{
							wtype = 2;
						}
						else
						{
							wtype = 0;
						}
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetZigZag32Arr (\"" + StrKey + "\", (vector<int32_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetZigZag32Arr(\"" + StrKey + "\", this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}

				}
				else if (_argType == "int64" || _argType == "uint64")
				{


					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 0;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetVarInt (\"" + StrKey + "\", (uint64_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetVarInt(\"" + StrKey + "\", (uint64_t)this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");

						_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						int wtype;
						if (!m_ParserProto2)
						{
							wtype = 2;
						}
						else
						{
							wtype = 0;
						}
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetVarIntArr (\"" + StrKey + "\", (vector<uint64_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetVarIntArr(\"" + StrKey + "\", *(vector<uint64_t>*)&this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}



				}
				else if (_argType == "sint64")
				{

					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 0;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetZigZag64 (\"" + StrKey + "\", (int64_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetZigZag64(\"" + StrKey + "\", (int64_t)&this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						int wtype;
						if (!m_ParserProto2)
						{
							wtype = 2;
						}
						else
						{
							wtype = 0;
						}
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetZigZag64Arr (\"" + StrKey + "\", (vector<uint64_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetZigZag64Arr(\"" + StrKey + "\", this->" + _argName + ");//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}

				}
				else if (_argType == "bytes")
				{
					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 2;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetBin (\"" + StrKey + "\", &this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetBin(\"" + StrKey + "\", this->" + _argName + ".data(),(int)this->" + _argName + ".size());//" + _argType + "\r\n");

						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{


						int	wtype = 2;

						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetBinArr (\"" + StrKey + "\", &this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");


						_Pack_code.append("\tm_pb->SetBinArr(\"" + StrKey + "\", this->" + _argName + ");//" + _argType + "\r\n");



						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}


				}
				else if (_argType == "string")
				{


					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 2;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetUTF (\"" + StrKey + "\", &this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetUTF(\"" + StrKey + "\", this->" + _argName + ".c_str());//" + _argType + "\r\n");

						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_string_tojsonstring(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{


						int	wtype = 2;

						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetUTFArr (\"" + StrKey + "\", &this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");


						//_Pack_code.append("\tm_pb->GetUTFArr(\"" + StrKey + "\", this->" + _argName + ");//" + _argType + "\r\n");
						_Pack_code.append("\tfor (size_t i = 0; i < this->" + _argName + ".size(); i++)\r\n\t{\r\n\t\tm_pb->SetUTF (\"" + StrKey + "\", this->" + _argName + "[i].c_str());" + "\r\n\t}\r\n");



						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}

				}
				else if (_argType == "float" || _argType == "fixed32" || _argType == "sfixed32") {

					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 5;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetInt (\"" + StrKey + "\", (uint32_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetInt(\"" + StrKey + "\", *((uint32_t*)&this->" + _argName + "));//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");

						_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						int wtype = 5;
						//wtype = 2;

						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetIntArr (\"" + StrKey + "\", (vector<uint32_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");


						_Pack_code.append("\tfor (size_t i = 0; i < this->" + _argName + ".size(); i++)\r\n\t{\r\n\t\tm_pb->SetInt (\"" + StrKey + "\", *((uint32_t*)&this->" + _argName + "[i]));" + "\r\n\t}\r\n");



						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}




				}
				else if (_argType == "double" || _argType == "sfixed64" || _argType == "fixed64") {

					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						int wtype = 1;
						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;
						_Unpack_code.append("\tm_unpb->GetInt64 (\"" + StrKey + "\", (uint64_t*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");
						_Pack_code.append("\tm_pb->SetInt64(\"" + StrKey + "\", *((uint64_t*)&this->" + _argName + "));//" + _argType + "\r\n");
						_Json_code.append("\"" + Json_argName + "\":%s,");

						_Json_arg_code.append("to_string(this->" + _argName + ").c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						int wtype = 1;
						//wtype = 2;

						int tag = (msgid << 3) | wtype;
						int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
						ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
						string StrKey = keyStrbyte;

						_Unpack_code.append("\tm_unpb->GetInt64Arr (\"" + StrKey + "\", (vector<uint64_t>*)&this->" + _argName + " , " + CheckArgstr + ");//" + _argType + "\r\n");


						_Pack_code.append("\tfor (size_t i = 0; i < this->" + _argName + ".size(); i++)\r\n\t{\r\n\t\tm_pb->SetInt64 (\"" + StrKey + "\", *((uint64_t*)&this->" + _argName + "[i]));" + "\r\n\t}\r\n");

						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pb_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}

				}
				else
				{

					//自定义类型
					int wtype = 2;
					int tag = (msgid << 3) | wtype;
					int _Klen = WriteVarint(tag, (unsigned char*)keybyte);
					ByteToHexStr((unsigned char*)keybyte, keyStrbyte, _Klen);
					string StrKey = keyStrbyte;

					if (type == msgArgsType::required || type == msgArgsType::optional)
					{
						_bin_count++;
						string _tmpname = "_tmpbin_" + to_string(_bin_count);
						_Unpack_code.append("\tvector<char> " + _tmpname + ";\r\n");
						_Build_tmpinfo _tmpinfo;
						_tmpinfo.IsArr = 0;
						_tmpinfo.SaveName = _tmpname;
						_tmpinfo.tmpArgName = "this->" + _argName;
						_tmpinfo.tmpType = _argType;
						BuildtmpinfoList.push_back(_tmpinfo);





						_Unpack_code.append("\tm_unpb->GetBin (\"" + StrKey + "\", &" + _tmpname + " , " + CheckArgstr + ");//" + _argType + " " + _argName + "\r\n");


						if (!IsWPacKLen)
						{
							_Pack_code.append("\tint _len = 0;\r\n\tconst char* _data = 0;\r\n");
							IsWPacKLen = 1;
						}
						_Pack_code.append("\t_data=this->" + _argName + ".Pack(&_len);\r\n");
						_Pack_code.append("\tm_pb->SetBin(\"" + StrKey + "\", _data, _len);\r\n");



						_Json_code.append("\"" + Json_argName + "\":%s,");

						_Json_arg_code.append("this->" + _argName + ".ToJson().c_str(),");

					}
					else if (type == msgArgsType::repeated)
					{
						_binArr_count++;
						string _tmpname = "_tmpbinarr_" + to_string(_binArr_count);
						_Unpack_code.append("\tvector <vector<char>> " + _tmpname + ";\r\n");
						_Build_tmpinfo _tmpinfo;
						_tmpinfo.IsArr = 1;
						_tmpinfo.SaveName = _tmpname;
						_tmpinfo.tmpArgName = "this->" + _argName;
						_tmpinfo.tmpType = _argType;
						BuildtmpinfoList.push_back(_tmpinfo);

						_Unpack_code.append("\tm_unpb->GetBinArr (\"" + StrKey + "\", &" + _tmpname + " , " + CheckArgstr + ");//" + _argType + " " + _argName + "\r\n");

						if (!IsWPacKLen)
						{
							_Pack_code.append("\tint _len = 0;\r\n\tconst char* _data = 0;\r\n");
							IsWPacKLen = 1;
						}






						_Pack_code.append("\tfor (size_t i = 0; i < this->" + _argName + ".size(); i++)\r\n\t{\r\n");

						_Pack_code.append("\t\t_data = this->" + _argName + "[i].Pack(&_len);\r\n");
						_Pack_code.append("\t\tm_pb->SetBin(\"" + StrKey + "\", _data, _len);\r\n\t}\r\n");


						_Json_code.append("\"" + Json_argName + "\":%s,");
						_Json_arg_code.append("pbobj_vector_tostring(this->" + _argName + ").c_str(),");

					}
					else
					{
						assert(0);
					}

				}


			}


			_Unpack_code.append("\tm_unpb->UnPack((const char*)data, len);\r\n");

			for (int j = 0; j < BuildtmpinfoList.size(); j++)
			{
				if (!BuildtmpinfoList[j].IsArr)
				{
					_Unpack_code.append("\t" + BuildtmpinfoList[j].tmpArgName + ".Parser(" + BuildtmpinfoList[j].SaveName + ".data(), (int)" + BuildtmpinfoList[j].SaveName + ".size());\r\n");

				}
				else
				{
					_Unpack_code.append("\t" + BuildtmpinfoList[j].tmpArgName + ".resize(" + BuildtmpinfoList[j].SaveName + ".size());\r\n");

					_Unpack_code.append("\tfor (size_t i = 0; i < " + BuildtmpinfoList[j].SaveName + ".size(); i++)\r\n\t{\r\n");

					_Unpack_code.append("\t\t" + BuildtmpinfoList[j].tmpArgName + "[i].Parser(" + BuildtmpinfoList[j].SaveName + "[i].data(), (int)" + BuildtmpinfoList[j].SaveName + "[i].size());\r\n");
					_Unpack_code.append("\t}\r\n");
				}
			}


			string _class_H_ = "";
			_class_H_.append("class " + Proto_messageList[i].Name + ":public pbobj\r\n{\r\n");
			_class_H_.append("public:\r\n");
			_class_H_.append("\t" + Proto_messageList[i].Name + "();\r\n");
			_class_H_.append("\t~" + Proto_messageList[i].Name + "(); \r\n");
			_class_H_.append("\tvoid Parser(const void* data, int len);\r\n");
			_class_H_.append("\tconst char* Pack(int* retlen);\r\n");
			_class_H_.append("\tvector<char> PackEx();\r\n");
			_class_H_.append("#if USER_TOJSON\r\n");
			_class_H_.append("\tstring ToJson();\r\n");
			_class_H_.append("#endif // USER_TOJSON\r\n");
			_class_H_.append(C_member);
			_class_H_.append("};\r\n");
			H_Code.append(_class_H_);
			string _class_CPP_ = "";

			//构造函数
			_class_CPP_.append(Proto_messageList[i].Name + "::" + Proto_messageList[i].Name + "()\r\n{\r\n");
			_class_CPP_.append("}\r\n");

			//析构函数
			_class_CPP_.append(Proto_messageList[i].Name + "::~" + Proto_messageList[i].Name + "()\r\n{\r\n");
			_class_CPP_.append("}\r\n");

			//Parser函数
			_class_CPP_.append("void "+Proto_messageList[i].Name + "::Parser(const void* data, int len)\r\n{\r\n");
			_class_CPP_.append("\tif (!m_unpb)m_unpb = new PbUnPack();\r\n");
			_class_CPP_.append(_Unpack_code);
			_class_CPP_.append("}\r\n");


			//Pack函数
			_class_CPP_.append("const char* " + Proto_messageList[i].Name + "::Pack(int* retlen)\r\n{\r\n");
			_class_CPP_.append("\tif (!m_pb)m_pb = new PbPackS();\r\n");
			_class_CPP_.append(_Pack_code);
			_class_CPP_.append("\tretlen[0]=this->m_pb->Pack();\r\n");
			_class_CPP_.append("\treturn this->m_pb->GetData();\r\n");
			_class_CPP_.append("}\r\n");
			//PackEx函数
			_class_CPP_.append("vector<char> "+Proto_messageList[i].Name + "::PackEx()\r\n{\r\n");
			_class_CPP_.append("\tint _len = 0;\r\n");
			_class_CPP_.append("\tconst char* _data=this->Pack(&_len);\r\n");
			_class_CPP_.append("\treturn pbobj_PackEx(_data, _len,\"" + Proto_messageList[i].Name + "\");\r\n");
			_class_CPP_.append("}\r\n");
			//ToJson函数
			_class_CPP_.append("#if USER_TOJSON\r\n");
			_class_CPP_.append("string "+Proto_messageList[i].Name + "::ToJson()\r\n{\r\n");
			if (_Json_code.size() && _Json_arg_code.size())
			{
				string _json_codestr = "{" + string(_Json_code.c_str(), _Json_code.size() - 1) + "}";
				_json_codestr = "\"" + string_replace(_json_codestr,"\"","\\\"")+"\"";
				string __json_argstr = string(_Json_arg_code.c_str(), _Json_arg_code.size() - 1);
				if (__json_argstr != "")
				{
					_class_CPP_.append("\tconst char* format = " + _json_codestr + ";\r\n");
					_class_CPP_.append("\treturn string_Format(format,");

					_class_CPP_.append(__json_argstr);
					_class_CPP_.append(");\r\n");
				}
				else
				{
					_class_CPP_.append("\treturn \"{}\";\r\n");
				}
			}
			else
			{
				_class_CPP_.append("\treturn \"{}\";\r\n");
			}



			//_class_CPP_.
			_class_CPP_.append("}\r\n");
			_class_CPP_.append("#endif // USER_TOJSON\r\n");
			CPP_Code.append(_class_CPP_);

		}
		else
		{
			assert(!"message Name 为空");
		}
	}

	string _dir = out_H_And_CPP_Dir;
	if (out_H_And_CPP_Dir[_dir.size() - 1] != '\\' && out_H_And_CPP_Dir[_dir.size() - 1] != '/')
	{
		_dir = _dir + "\\";
	}
	string _H_F = _dir + out_H_And_CPP_name + ".h";

	FILE* fp;
	fp = fopen(_H_F.c_str(), "wb");
	if (fp)
	{
		fwrite(H_Code.c_str(), 1, H_Code.size(), fp);
		fclose(fp);
	}
	string _CPP_F = _dir + out_H_And_CPP_name + ".cpp";
	fp = fopen(_CPP_F.c_str(), "wb");
	if (fp)
	{
		fwrite(CPP_Code.c_str(), 1, CPP_Code.size(), fp);
		fclose(fp);
	}
}

#endif
