#include <iostream>
#include <ranges>
#include <unordered_map>
#include <filesystem>
#include <Rut/RxMem.h>
#include <Seraph/Script.h>


static std::unordered_map<size_t, std::wstring> GetNameTable(const std::filesystem::path phNameTable, const std::wstring& wsGameTitle)
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

int main()
{
	TestParse();
}