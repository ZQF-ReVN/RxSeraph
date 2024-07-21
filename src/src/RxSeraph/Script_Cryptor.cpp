#include <RxSeraph/Script_Cryptor.h>
#include <ZxJson/JDoc.h>
#include <filesystem>


namespace ZQF::RxSeraph::Script
{
    Cryptor::Cryptor()
    {

    }

    auto Cryptor::LZ77Dec(std::uint8_t* pRaw, std::uint8_t* pDec) -> std::size_t
    {
        std::size_t ite_size = 0;
        std::size_t dec_size = (std::size_t)(*((std::uint32_t*)pRaw));

        std::uint8_t* dec_ptr = pDec;
        std::uint8_t* enc_ptr = pRaw + 4;

        if (dec_size == 0) { return 0; }

        do
        {
            std::uint8_t token = *enc_ptr++;
            if (token & 0x80) // expand
            {
                std::uint32_t expand_info = *enc_ptr++ + (token << 8);

                std::uint32_t copy_size = (expand_info & 0x1F) + 1;
                ite_size += copy_size;

                std::uint32_t back_offset = (std::uint16_t)(((expand_info >> 5) & 0x3FF) + 1);
                std::uint8_t* back_ptr = dec_ptr - back_offset;

                do
                {
                    *dec_ptr++ = *back_ptr++;
                } while (--copy_size);
            }
            else // just copy 
            {
                std::uint32_t copy_size = (token & 0x7F) + 1;
                ite_size += copy_size;

                do
                {
                    *dec_ptr++ = *enc_ptr++;
                } while (--copy_size);
            }

        } while (ite_size < dec_size);

        return ite_size;
    }


    auto Cryptor::Dec(ZxMem& rScript, ZxMem& rDec, const std::size_t nMaxDecSize) -> bool
    {
        rDec.Resize(nMaxDecSize);
        std::size_t size = Cryptor::LZ77Dec(rScript.Ptr(), rDec.Ptr());
        rDec.Resize(size);
        return size ? true : false;
    }

    auto Cryptor::GetFilterSet(const std::string_view msFilterPath, const std::string_view msGameName) -> std::unordered_set<std::string>
    {
        ZxJson::JDoc filter_json{ msFilterPath };
        auto& filter_map = filter_json.GetJObject();

        const auto find_ite = filter_map.find(msGameName);
        if (find_ite == filter_map.end()) { throw std::runtime_error("GetFilterSet: not find target game filter obj"); }

        std::unordered_set<std::string> filter_set;
        for (auto& val : find_ite->second.Get<ZxJson::JArray_t&>())
        {
            filter_set.insert(val.Get<std::string>());
        }

        return filter_set;
    }

    auto Cryptor::BatchDec(const std::string_view msScriptPath, const std::string_view msSavePath, const std::unordered_set<std::string>& stFilter) -> void
    {
        ZxMem tmp_mem(0x64000);
        ZxMem script_mem(0x64000);

        std::filesystem::create_directories(msSavePath);

        for (const auto& path_entry : std::filesystem::directory_iterator(msScriptPath))
        {
            if (path_entry.is_regular_file() == false) { continue; }

            std::filesystem::path target_path = msSavePath / path_entry.path().filename().replace_extension(L".scn");

            if (stFilter.find(path_entry.path().filename().string()) != stFilter.end())
            {
                if (std::filesystem::exists(target_path))
                {
                    std::filesystem::remove(target_path);
                }

                std::filesystem::copy(path_entry, target_path);
            }
            else
            {
                script_mem.Load(path_entry.path().string());
                Cryptor::Dec(script_mem, tmp_mem);
                tmp_mem.Save(target_path.string());
            }
        }
    }

    auto Cryptor::BatchDec(const std::string_view msScriptPath, const std::string_view msSavePath, const std::string_view msFilterPath, const std::string_view msGameName) -> void
    {
        Cryptor::BatchDec(msScriptPath, msSavePath, Cryptor::GetFilterSet(msFilterPath, msGameName));
    }
}
