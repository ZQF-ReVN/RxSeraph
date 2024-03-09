#include <iostream>
#include <ranges>
#include <unordered_map>
#include <filesystem>
#include <Rut/RxMem.h>
#include <Rut/RxFile.h>
#include <Seraph/Script.h>


static std::unordered_map<size_t, std::wstring> GetNameTable(const std::filesystem::path& phNameTable, const std::wstring& wsGameTitle)
{
	Rut::RxJson::JValue name_table_list_json = Rut::RxJson::Parser{}.Load(phNameTable);
	Rut::RxJson::JObject& name_table_list_jobj = name_table_list_json.ToOBJ();
	Rut::RxJson::JArray& name_table_jarray = name_table_list_jobj[wsGameTitle];

	std::unordered_map<size_t, std::wstring> name_table;
	for (auto [seq, val] : std::views::enumerate(name_table_jarray))
	{
		name_table[seq] = val.ToStrView();
	}

	return name_table;
}

static void Export(const std::filesystem::path& phJson, const std::filesystem::path& phSave)
{
	std::wstring txt;
	std::wstring_view char_name;
	Rut::RxJson::JValue codes_json = Rut::RxJson::Parser{}.Load(phJson);
	for (auto& code : codes_json.ToAry())
	{
		std::wstring_view code_name = code[L"self"].ToStrView();
		if (code_name == L"character")
		{
			char_name = code[L"name"].ToStrView();

		}
		else if (code_name == L"msg")
		{
			txt.append(L"*msg:");
			txt.append(std::to_wstring(code[L"begin"].ToInt()));
			txt.append(1, L'\n');
			if (char_name != L"null")
			{
				txt.append(1, L'【');
				txt.append(char_name);
				txt.append(1, L'】');
			}
			char_name = L"";

			for (auto& msg_code : code[L"texts"].ToAry())
			{
				Rut::RxJson::JObject& msg_code_obj = msg_code.ToOBJ();
				switch (msg_code_obj[L"opcode"].ToInt())
				{
				case 0x00: txt.append(msg_code_obj[L"parameter"][L"value"]); break;
				case 0x14: txt.append(L"[n]"); break;
				case 0x17: txt.append(L"\n"); break;
				case 0xFF: txt.append(L"\n\n"); break;
				}
			}
		}
		else if (code_name == L"select")
		{
			txt.append(L"*select:\n");
			for (auto& sel_text : code[L"texts"].ToAry())
			{
				txt.append(sel_text);
				txt.append(1, L'\n');
			}
			txt.append(1, L'\n');
		}
	}

	Rut::RxFile::Text ofs{ phSave, Rut::RIO_WRITE, Rut::RFM_UTF8 };
	ofs << txt;
}

static void TestParse()
{
	std::unordered_map<size_t, std::wstring> name_table = GetNameTable(L"name_table.json", L"[061215][EX12] 雛鳥の堕ちる音");

	Rut::RxMem::Auto script_mem;
	std::filesystem::path save_folder = L"json/";
	std::filesystem::create_directories(save_folder);
	for (const auto& path_entry : std::filesystem::directory_iterator("scn_dec/"))
	{
		if (path_entry.is_regular_file() == false) { continue; }

		script_mem.LoadFile(path_entry);
		Seraph::Script::V2::Parser script(script_mem);
		Rut::RxJson::JValue codes = script.Parse(name_table);
		Rut::RxJson::Parser::Save(codes, save_folder / path_entry.path().filename().replace_extension(L".json"), true);
	}
}

static void ExportTBatch()
{
	std::filesystem::path save_folder = L"human/";
	std::filesystem::create_directories(save_folder);
	for (const auto& path_entry : std::filesystem::directory_iterator("json/"))
	{
		if (path_entry.is_regular_file() == false) { continue; }
		Export(path_entry, save_folder / path_entry.path().filename().replace_extension(L".txt"));
	}
}

int main()
{
	ExportTBatch();
	//TestParse();
}