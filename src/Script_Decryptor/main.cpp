#include <iostream>
#include <Rut/RxCmd.h>
#include <Seraph/Script_Cryptor.h>


static void UserMain(int argc, wchar_t* argv[])
{
	Rut::RxCmd::Arg arg;
	arg.AddCmd(L"-scn", L"encrypted files dir");
	arg.AddCmd(L"-out", L"save dir");
	arg.AddCmd(L"-filter", L"filter json path");
	arg.AddCmd(L"-name", L"game name in filter json file");
	arg.AddCmd(L"-mode", L"mode: [batch]");
	arg.AddExample(L"-mode batch -scn ScnPac/ -out scn_dec/ -filter script_filter.json -name \"[061215][EX12] 雛鳥の堕ちる音\"");
	if (arg.Load(argc, argv) == false) { return; }

	if (arg[L"-mode"] == L"batch")
	{
		Seraph::Script::Crypt::BatchDec(arg[L"-scn"], arg[L"-out"], arg[L"-filter"], arg[L"-name"]);
	}
	else
	{
		throw std::runtime_error("Command Error!");
	}
}

int wmain(int argc, wchar_t* argv[])
{
	try
	{
		::UserMain(argc, argv);
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
	}
}