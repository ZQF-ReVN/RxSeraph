#include <print>
#include <iostream>
#include <ranges>
#include <ZxFS/ZxFS.h>
#include <ZxFile/ZxFile.h>
#include <ZxJson/JIO.h>
#include <ZxJson/JValue.h>
#include <ZxJson/JParser.h>
#include <RxSeraph/Pack.h>
#include <RxSeraph/Script.h>
#include <RxSeraph/Script_Cryptor.h>


[[maybe_unused]] static auto LoadNameTable(const std::string_view msNameTablePath, const std::string_view msGameTitle) -> std::vector<std::string>
{
    auto json_name_table = ZQF::ZxJson::LoadViaFile(msNameTablePath);

    std::vector<std::string> name_table;
    for (auto& name : json_name_table[std::string(msGameTitle)].Get<ZQF::ZxJson::JArray_t&>())
    {
        name_table.emplace_back(name.Get<std::string>());
    }

    return name_table;
}

[[maybe_unused]] static auto ExportTextViaJson(const std::string_view msJsonPath, const std::string_view msSavePath) -> void
{
    std::string txt;
    std::string_view char_name;
    auto json_codes = ZQF::ZxJson::LoadViaFile(msJsonPath);
    for (auto& code : json_codes.Get<ZQF::ZxJson::JArray_t&>())
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
                case 0x17: txt.append(1, '\n'); break;
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

[[maybe_unused]] static auto ParseToJson(const std::string_view msScriptDir, const std::string_view msSaveDir, const std::size_t nCodePage, std::vector<std::string>& rfNameTable) -> void
{
    std::string path_tmp;
    ZQF::ZxMem script_mem;
    ZQF::ZxFS::DirMake(msSaveDir, false);
    for (ZQF::ZxFS::Walk walk{ msScriptDir }; walk.NextFile();)
    {
        if (!walk.IsSuffix(".scn")) { continue; }
        ZQF::RxSeraph::Script::V2::Parser script_parser(script_mem.Load(walk.GetPath()).Span(), nCodePage);
        auto script_jarry = script_parser.Parse(rfNameTable);
        if (script_jarry.empty()) { continue; }

        auto& save_path = path_tmp.append(msSaveDir).append(ZQF::ZxFS::FileSuffixDel(walk.GetName())).append(".json");
        ZQF::ZxJson::JValue json_val{ std::move(script_jarry) };
        ZQF::ZxJson::StoreViaFile(save_path, json_val, true, true);
        path_tmp.clear();
    }
}

[[maybe_unused]] static auto TestScenarioMake() -> void
{

}

[[maybe_unused]] static auto JsonToTXT(const std::string_view msJsonDir, const std::string_view msSaveDir) -> void
{
    std::string path_tmp;
    ZQF::ZxFS::DirMake(msSaveDir, false);
    for (ZQF::ZxFS::Walk walk{ msJsonDir }; walk.NextFile();)
    {
        ExportTextViaJson(walk.GetPath(), path_tmp.append(msSaveDir).append(ZQF::ZxFS::FileSuffixDel(walk.GetName())).append(".txt"));
        path_tmp.clear();
    }
}


auto main() -> int
{
    try
    {
        ZQF::RxSeraph::Pack::Export("ScnPac.Dat", "ScnPac/", 0x0);
        ZQF::RxSeraph::Script::Cryptor::BatchDec("ScnPac/", "scn_dec/", "script_filter.json", "[061215][EX12] 雛鳥の堕ちる音");
        std::vector<std::string> name_table = LoadNameTable("name_table.json", "[061215][EX12] 雛鳥の堕ちる音");
        ParseToJson("scn_dec/", "json/", 932, name_table);
        JsonToTXT("json/", "human/");
        
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
