#include <filesystem>
#include <unordered_set>
#include <Rut/RxMem.h>
#include <Rut/RxCmd.h>
#include <Rut/RxJson.h>
#include <Seraph/Script_Cryptor.h>


static void BatchDecompress(const std::filesystem::path& phScript, const std::filesystem::path& phSave, const std::unordered_set<std::wstring>& stFilter)
{
	Rut::RxMem::Auto tmp_mem(0x64000);
	Rut::RxMem::Auto script_mem(0x64000);

	std::filesystem::create_directories(phSave);

	for (const auto& path_entry : std::filesystem::directory_iterator(phScript))
	{
		if (path_entry.is_regular_file() == false) { continue; }

		std::filesystem::path target_path = phSave / path_entry.path().filename().replace_extension(L".scn");

		if (stFilter.find(path_entry.path().filename()) != stFilter.end())
		{
			if (std::filesystem::exists(target_path))
			{
				std::filesystem::remove(target_path);
			}

			std::filesystem::copy(path_entry, target_path);
		}
		else
		{
			script_mem.LoadFile(path_entry);
			Seraph::Script::Crypt::Dec(script_mem, tmp_mem);
			tmp_mem.SaveData(target_path);
		}
	}
}

static std::unordered_set<std::wstring> GetFilterSet(const std::wstring& wsGameName)
{
	Rut::RxJson::JValue filter_json = Rut::RxJson::Parser{}.Load(L"Script_Decryptor_Filter.json");
	Rut::RxJson::JObject& filter_map = filter_json.ToOBJ();

	auto find_ite = filter_map.find(wsGameName);
	if (find_ite == filter_map.end()) { throw std::runtime_error("GetFilterSet: not find target game filter obj"); }

	std::unordered_set<std::wstring> filter_set;
	filter_set.insert_range((*find_ite).second.ToAry());

	return filter_set;
}

static void UserMain(int argc, wchar_t* argv[])
{

}

int main()
{
	BatchDecompress(L"ScnPac/", L"scn_dec/", GetFilterSet(L"[000714][Ather] 好きだよっ"));
}