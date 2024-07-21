#include <iostream>
#include <ranges>
#include <unordered_map>
#include <filesystem>
#include <ZxJson/JValue.h>
#include <ZxJson/JParser.h>
#include <ZxJson/JDoc.h>
#include <ZxFile/ZxFile.h>
#include <RxSeraph/Script.h>
#include <RxSeraph/Script_Cryptor.h>


[[maybe_unused]]  static auto GetNameTable(const std::string_view msNameTablePath, const std::string_view msGameTitle) -> std::vector<std::string>
{
    ZQF::ZxJson::JDoc name_table_doc{ msNameTablePath };

	std::vector<std::string> name_table;
	for (auto& name : name_table_doc.GetJObject()[msGameTitle.data()].Get<ZQF::ZxJson::JArray_t&>())
	{
		name_table.emplace_back(name.Get<std::string>());
	}

	return name_table;
}

[[maybe_unused]]  static auto ExportText(const std::string_view msJsonPath, const std::string_view msSavePath) -> void
{
	std::string txt;
	std::string_view char_name;
    ZQF::ZxJson::JDoc codes_json{ msJsonPath };
	for (auto& code : codes_json.GetJArray())
	{
		std::string_view code_name = code["self"].Get<std::string_view>();
		if (code_name == "character")
		{
			char_name = code["name"].Get<std::string_view>();

		}
		else if (code_name == "msg")
		{
			txt.append("*msg:");
			txt.append(std::to_string(code["begin"].Get<size_t>()));
			txt.append(1, '\n');
			if (char_name.size())
			{
				txt.append("【");
				txt.append(char_name);
				txt.append("】");
			}
			char_name = "";

			for (auto& msg_code : code["texts"].Get<ZQF::ZxJson::JArray_t&>())
			{
				auto& msg_code_obj = msg_code.Get<ZQF::ZxJson::JObject_t&>();
				switch (msg_code_obj["opcode"].Get<uint8_t>())
				{
                case 0x00: txt.append(msg_code_obj["parameter"]["value"].Get<std::string_view>()); break;
				case 0x14: txt.append("[n]"); break;
				case 0x17: txt.append(1,'\n'); break;
				case 0xFF: txt.append("\n\n"); break;
				}
			}
		}
		else if (code_name == "select")
		{
			txt.append("*select:\n");
			for (auto& sel_text : code["texts"].Get<ZQF::ZxJson::JArray_t&>())
			{
				txt.append(sel_text.Get<std::string_view>());
				txt.append(1, '\n');
			}
			txt.append(1, '\n');
		}
	}

    ZQF::ZxFile{ msSavePath, ZQF::ZxFile::OpenMod::WriteForce } << txt;
}

[[maybe_unused]]  static auto TestParse() -> int
{
	std::vector<std::string> name_table = GetNameTable("name_table.json", "[061215][EX12] 雛鳥の堕ちる音");

	ZQF::ZxMem script_mem;
	std::filesystem::path save_folder = L"json/";
	std::filesystem::create_directories(save_folder);
	for (const auto& path_entry : std::filesystem::directory_iterator("scn_dec/"))
	{
		if (path_entry.is_regular_file() == false) { continue; }

		/*script_mem.Load(path_entry.path().string());*/
        script_mem.Load(path_entry.path().string());
		ZQF::RxSeraph::Script::V2::Parser script(script_mem.Span(), 932);
        ZQF::ZxJson::JDoc jdoc;
        jdoc.GetJValue() = script.Parse(name_table);
        jdoc.StoreViaFile((save_folder / path_entry.path().filename().replace_extension(".json")).string(), true, true);
	}
}

[[maybe_unused]] static auto TestScenarioMake() -> int
{

}

[[maybe_unused]] static auto ExportBatch() -> int
{
	std::filesystem::path save_folder = "human/";
	std::filesystem::create_directories(save_folder);
	for (const auto& path_entry : std::filesystem::directory_iterator("json/"))
	{
		if (path_entry.is_regular_file() == false) { continue; }
        ExportText(path_entry.path().string(), (save_folder / path_entry.path().filename().replace_extension(".txt")).string());
	}
}



auto main() -> int
{
    try
    {
        TestParse();
        // ExportBatch();
        //ZQF::RxSeraph::Script::Cryptor::BatchDec("ScnPac/", "scn_dec/", "script_filter.json", "[061215][EX12] 雛鳥の堕ちる音");
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
