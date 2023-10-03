# protobuf-PbParser
PbParser是一个VisualStudio2022项目,用于解析和打包protobuf的二进制数据,它允许在没有proto文件的情况下,准确的解析和打包数据

//PbParser是一个VisualStudio2022项目,用于解析和打包protobuf的二进制数据,它允许在没有proto文件的情况下,准确的解析和打包数据
//PbParser不依赖官方protobuf,十分小巧,它主要使用pbUtil\PbUnPack和PbPackS这3个class


//在没有proto文件的情况下,需要通过pbUtil下的GetPackCode函数获取打包代码,也可以通过同样的方式进行解析

//下面是一个简单的打包例子

//PbPackS pb;
//pb.SetVarIntArr("12", { 12345678910,12345678 });
//pb.SetStr("7A[0]", "123456");
//pb.SetStr("7A[1]", "56789");
//int _len = pb.Pack();
//const char* data = pb.GetData();

//与例子对应的解析代码--注意,在使用UnPack之前,需保证变量没有被释放
//PbUnPack pb;
//vector<uint64_t> _arr;
//pb.GetVarIntArr("12",&_arr);
//string _srt1;
//string _srt2;
//pb.GetStr("7A[0]",&_srt1);
//pb.GetStr("7A[1]",&_srt2);
//pb.UnPack(data, _len);

//若拥有proto文件,建议使用官方项目https://github.com/protocolbuffers/protobuf,
//如果需要使用PbParser,可以尝试通过pbUtil下的protoFileToCode函数生成代码
//protoFileToCode函数并不完善,并不能保证生成的代码100%可用,我会一直尝试修复这个函数,如果能给我反馈BUG或提供解决方案,我将十分感激


//PbParser,通过节点方式储存和解析数据,父节点均可以使用GetBin取出.节点中存在[]表示数组,可以通过GetxxxArr取出,也可以添加[xx]指定第xx个成员
//对于proto2/proto3语法的数组数据,均可以通过GetxxxArr及SetxxxArr解析及打包

//若无法成功取出数据,建议先使用GetPackCode查看节点结构

//TestMain.cpp下有几个简单的例子,可以通过选择项目配置来启用

//联系方式:QQ 2694037160 请备注来意





//PbParser is a VisualStudio2022 project for parsing and packaging protobuf binary data, it allows accurate parsing and packaging of data without proto files
//PbParser does not rely on the official protobuf. It is very small and uses 3 classes: pbUtil\PbUnPack and PbPackS

// In case there is no proto file, the GetPackCode function in pbUtil can be used to fetch the package code, which can be parsed in the same way

// Here's a simple bundling example
//PbPackS pb;
//pb.SetVarIntArr("12", { 12345678910,12345678 });
//pb.SetStr("7A[0]", "123456");
//pb.SetStr("7A[1]", "56789");
//int _len = pb.Pack();
//const char* data = pb.GetData();

// Parsing code for example - Note that you need to make sure the variable is not freed before using UnPack
//PbUnPack pb;
//vector<uint64_t> _arr;
//pb.GetVarIntArr("12",&_arr);
//string _srt1;
//string _srt2;
//pb.GetStr("7A[0]",&_srt1);
//pb.GetStr("7A[1]",&_srt2);
//pb.UnPack(data, _len);

// if have proto file, it is recommended to use the official project https://github.com/protocolbuffers/protobuf,
// If you need to use PbParser, try generating code via protoFileToCode in pbUtil
//protoFileToCode is not perfect and the generated code is not guaranteed to be 100% usable, I will always try to fix this function, if you can give me BUG feedback or provide a solution, I would be very grateful


//PbParser, which stores and parses data in a node way that the parent node can fetch using GetBin. The presence of [] in a node represents an array, which can be retrieved using GetxxxArr, or [xx] can be added to specify the xx member
// Array data of proto2/proto3 syntax can be parsed and packed by GetxxxArr and SetxxxArr
// If we were unable to retrieve the data, we recommend using GetPackCode to view the node structure first

//TestMain.cpp has a few simple examples that can be enabled by selecting project config
// Contact information :QQ 2694037160 Please note the purpose of the visit

以上翻译结果来自有道神经网络翻译（YNMT）· 计算机
