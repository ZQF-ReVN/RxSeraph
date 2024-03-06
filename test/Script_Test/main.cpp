#include <iostream>
#include <filesystem>
#include <Rut/RxMem.h>
#include <Seraph/Script.h>


int main()
{
	Rut::RxMem::Auto script_mem;
	std::filesystem::path save_folder = L"json/";
	std::filesystem::create_directories(save_folder);
	for (const auto& path_entry : std::filesystem::directory_iterator("scn/"))
	{
		if (path_entry.is_regular_file() == false) { continue; }
		
		script_mem.LoadFile(path_entry);
		Seraph::Script::Parser script(script_mem);
		Rut::RxJson::JValue codes = script.Parse();
		Rut::RxJson::Parser::Save(codes, save_folder / path_entry.path().filename().replace_extension(L".json"), true);
	}
}