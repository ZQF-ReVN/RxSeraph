#pragma once
#include <cstdint>
#include <unordered_set>
#include <Rut/RxMem.h>
#include <Rut/RxJson.h>


namespace Seraph::Script
{
	class Crypt
	{
	public:
		Crypt()
		{

		}

		static size_t LZ77Dec(uint8_t* pRaw, uint8_t* pDec)
		{
			size_t ite_size = 0;
			size_t dec_size = (size_t)(*((uint32_t*)pRaw));

			uint8_t* dec_ptr = pDec;
			uint8_t* enc_ptr = pRaw + 4;

			if (dec_size == 0) { return 0; }

			do
			{
				uint8_t token = *enc_ptr++;
				if (token & 0x80) // expand
				{
					uint32_t expand_info = *enc_ptr++ + (token << 8);

					uint32_t copy_size = (expand_info & 0x1F) + 1;
					ite_size += copy_size;

					uint32_t back_offset = (uint16_t)(((expand_info >> 5) & 0x3FF) + 1);
					uint8_t* back_ptr = pDec - back_offset;

					do
					{
						*pDec++ = *back_ptr++;
					} while (--copy_size);
				}
				else // just copy 
				{
					uint32_t copy_size = (token & 0x7F) + 1;
					ite_size += copy_size;

					do
					{
						*pDec++ = *enc_ptr++;
					} while (--copy_size);
				}

			} while (ite_size < dec_size);

			return ite_size;
		}

		static bool Dec(Rut::RxMem::Auto& rScript, Rut::RxMem::Auto& rDec, size_t nMaxDecSize = 0x64000)
		{
			rDec.SetSize(nMaxDecSize);
			size_t size = Crypt::LZ77Dec(rScript.GetPtr(), rDec.GetPtr());
			rDec.SetSize(size);
			return size ? true : false;
		}

		static std::unordered_set<std::wstring> GetFilterSet(const std::filesystem::path phFilter, const std::wstring& wsGameName)
		{
			Rut::RxJson::JValue filter_json = Rut::RxJson::Parser{}.Load(phFilter);
			Rut::RxJson::JObject& filter_map = filter_json.ToOBJ();

			auto find_ite = filter_map.find(wsGameName);
			if (find_ite == filter_map.end()) { throw std::runtime_error("GetFilterSet: not find target game filter obj"); }

			std::unordered_set<std::wstring> filter_set;
			filter_set.insert_range((*find_ite).second.ToAry());

			return filter_set;
		}

		static void BatchDec(const std::filesystem::path& phScript, const std::filesystem::path& phSave, const std::unordered_set<std::wstring>& stFilter)
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
					Crypt::Dec(script_mem, tmp_mem);
					tmp_mem.SaveData(target_path);
				}
			}
		}

		static void BatchDec(const std::filesystem::path& phScript, const std::filesystem::path& phSave, const std::filesystem::path phFilter, const std::wstring& wsGameName)
		{
			Crypt::BatchDec(phScript, phSave, Crypt::GetFilterSet(phFilter, wsGameName));
		}
	};

}