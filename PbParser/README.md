//PbParser��һ��VisualStudio2022��Ŀ,���ڽ����ʹ��protobuf�Ķ���������,��������û��proto�ļ��������,׼ȷ�Ľ����ʹ������
//PbParser�������ٷ�protobuf,ʮ��С��,����Ҫʹ��pbUtil\PbUnPack��PbPackS��3��class


//��û��proto�ļ��������,��Ҫͨ��pbUtil�µ�GetPackCode������ȡ�������,Ҳ����ͨ��ͬ���ķ�ʽ���н���

//������һ���򵥵Ĵ������
PbPackS pb;
pb.SetVarIntArr("12", { 12345678910,12345678 });
pb.SetStr("7A[0]", "123456");
pb.SetStr("7A[1]", "56789");
int _len = pb.Pack();
const char* data = pb.GetData();
//�����Ӷ�Ӧ�Ľ�������--ע��,��ʹ��UnPack֮ǰ,�豣֤����û�б��ͷ�
PbUnPack pb;
vector<uint64_t> _arr;
pb.GetVarIntArr("12",&_arr);
string _srt1;
string _srt2;
pb.GetStr("7A[0]",&_srt1);
pb.GetStr("7A[1]",&_srt2);
pb.UnPack(data, _len);

//��ӵ��proto�ļ�,����ʹ�ùٷ���Ŀhttps://github.com/protocolbuffers/protobuf,
//�����Ҫʹ��PbParser,���Գ���ͨ��pbUtil�µ�protoFileToCode�������ɴ���
//protoFileToCode������������,�����ܱ�֤���ɵĴ���100%����,�һ�һֱ�����޸��������,����ܸ��ҷ���BUG���ṩ�������,�ҽ�ʮ�ָм�


//PbParser,ͨ���ڵ㷽ʽ����ͽ�������,���ڵ������ʹ��GetBinȡ��.�ڵ��д���[]��ʾ����,����ͨ��GetxxxArrȡ��,Ҳ�������[xx]ָ����xx����Ա
//����proto2/proto3�﷨����������,������ͨ��GetxxxArr��SetxxxArr���������

//���޷��ɹ�ȡ������,������ʹ��GetPackCode�鿴�ڵ�ṹ

//TestMain.cpp���м����򵥵�����,����ͨ��ѡ����Ŀ����������

//��ϵ��ʽ:QQ 2694037160 �뱸ע����





//PbParser is a VisualStudio2022 project for parsing and packaging protobuf binary data, it allows accurate parsing and packaging of data without proto files
//PbParser does not rely on the official protobuf. It is very small and uses 3 classes: pbUtil\PbUnPack and PbPackS

// In case there is no proto file, the GetPackCode function in pbUtil can be used to fetch the package code, which can be parsed in the same way

// Here's a simple bundling example
PbPackS pb;
pb.SetVarIntArr("12", { 12345678910,12345678 });
pb.SetStr("7A[0]", "123456");
pb.SetStr("7A[1]", "56789");
int _len = pb.Pack();
const char* data = pb.GetData();
// Parsing code for example - Note that you need to make sure the variable is not freed before using UnPack
PbUnPack pb;
vector<uint64_t> _arr;
pb.GetVarIntArr("12",&_arr);
string _srt1;
string _srt2;
pb.GetStr("7A[0]",&_srt1);
pb.GetStr("7A[1]",&_srt2);
pb.UnPack(data, _len);

// if have proto file, it is recommended to use the official project https://github.com/protocolbuffers/protobuf,
// If you need to use PbParser, try generating code via protoFileToCode in pbUtil
//protoFileToCode is not perfect and the generated code is not guaranteed to be 100% usable, I will always try to fix this function, if you can give me BUG feedback or provide a solution, I would be very grateful


//PbParser, which stores and parses data in a node way that the parent node can fetch using GetBin. The presence of [] in a node represents an array, which can be retrieved using GetxxxArr, or [xx] can be added to specify the xx member
// Array data of proto2/proto3 syntax can be parsed and packed by GetxxxArr and SetxxxArr
// If we were unable to retrieve the data, we recommend using GetPackCode to view the node structure first

//TestMain.cpp has a few simple examples that can be enabled by selecting project config
// Contact information :QQ 2694037160 Please note the purpose of the visit

���Ϸ����������е������緭�루YNMT���� �����








 


