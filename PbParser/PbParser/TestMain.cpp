#include "pch.h"
#ifdef  _TEST_
#include "pbUtil.h"
#include "PbPackS.h"
#include "PbUnPack.h"
void pbUtiltest();
void pbpacktest();
void pbunpacktest();
//const char* HexStr = "0864126d08f4f77d10e28c02187e2001280130003800400148005000621110ffffffffffffffffff0118f4f77d2001621110ffffffffffffffffff0118f4f77d2002621110ffffffffffffffffff0118f4f77d200370007d00000000880100900100980100a001ffffffffffffffffff01122508c3a3bc011081af0118592001280230003800400148005000880100900100980100a00100124a08c0efc10110a48e01183b20012801300038004001480050005a2308c0efc10110a48e01180020002a08089d04150ad7a33c2a0808f803150080fa433000880100900100980100a00100127208c0efc20110ecb30218d0012001280130003800400148005000621210ffffffffffffffffff0118c0efc2012001621210ffffffffffffffffff0118c0efc2012002621210ffffffffffffffffff0118c0efc201200370007d00000000880100900100980100a001ffffffffffffffffff01127208d3c780011083ac0218b4012001280130003800400148005000621210ffffffffffffffffff0118d3c780012001621210ffffffffffffffffff0118d3c780012002621210ffffffffffffffffff0118d3c78001200370007d00000000880100900100980100a001ffffffffffffffffff01122608c3bfc00110d0910318e3012001280230003800400148005000880100900100980100a00100127108dd87970110e18c02187a2001280130003800400148005000621210ffffffffffffffffff0118dd8797012001621210ffffffffffffffffff0118dd8797012002621210ffffffffffffffffff0118dd879701200370007d00000000880100900100980100a001ffffffffffffffffff01122608dc8797011089b60718b2022001280130003800400148005000880100900100980100a00100127208c8afc40110ecb30218d2012001280130003800400148005000621210ffffffffffffffffff0118c8afc4012001621210ffffffffffffffffff0118c8afc4012002621210ffffffffffffffffff0118c8afc401200370007d00000000880100900100980100a001ffffffffffffffffff01122508d497b50110bdd0051805200e280230003800400148005000880100900100980100a00100";
const char* HexStr = "1209BEB8F0FE2DCEC2F1057A063132333435367A053536373839";
int main() 
{
	pbUtiltest();
	pbpacktest();
	pbunpacktest();
	system("pause");
	return 0;
}




void pbUtiltest()
{
	unsigned char bytes[102400] = { 0 };
	int len = HexToBin((char*)HexStr, bytes);
	pbUtil m_pb;
	const char*  code = m_pb.GetPackCode((char*)bytes, len);
	printf("%s\r\n", code);

	m_pb.protoFileToCode("../test_proto/any_test.proto","../test_proto/", "test");
	m_pb.protoFileToCode("../test_proto/any_test.proto", "../test_proto/", "test2",NULL,0,1);
}

void pbpacktest()
{
	PbPackS pb;
	pb.SetVarIntArr("12", { 12345678910,12345678 });
	pb.SetStr("7A[0]", "123456");
	pb.SetStr("7A[1]", "56789");

	int _len = pb.Pack();
	const char* code = pb.GetData();
	char aa[102400] = { 0 };
	ByteToHexStr((unsigned char*)code, aa, _len);
	printf("%s\r\n", aa);
}

void pbunpacktest()
{
	unsigned char bytes[102400] = { 0 };
	int len = HexToBin((char*)HexStr, bytes);
	PbUnPack pb;
	vector<uint64_t> _arr;
	pb.GetVarIntArr("12",&_arr);
	string _srt1;
	string _srt2;
	pb.GetStr("7A[0]",&_srt1);
	pb.GetStr("7A[1]",&_srt2);
	pb.UnPack(bytes, len);
	
	
}
#endif //  _TEST_