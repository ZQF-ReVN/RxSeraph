#include <RxSeraph/Script_Cryptor.h>
#include <ZxJson/JDoc.h>
#include <ZxJson/JIO.h>
#include <ZxFS/ZxFS.h>


namespace ZQF::RxSeraph::Script
{
    Cryptor::Cryptor()
    {

    }

    auto Cryptor::LZ77Dec(std::uint8_t* pRaw, std::uint8_t* pDec) -> std::size_t
    {
        std::size_t ite_size = 0;
        std::size_t dec_size = static_cast<std::size_t>(*reinterpret_cast<std::uint32_t*>(pRaw));

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

                std::uint32_t back_offset = static_cast<std::uint16_t>((expand_info >> 5) & 0x3FF) + 1;
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
        const auto dec_size = Cryptor::LZ77Dec(rScript.Ptr<std::uint8_t*>(), rDec.Ptr<std::uint8_t*>());
        rDec.Resize(dec_size, false, true);
        return dec_size ? true : false;
    }

    auto Cryptor::GetFilterSet(const std::string_view msFilterPath, const std::string_view msGameName) -> std::unordered_set<std::string>
    {
        const auto filter_json = ZQF::ZxJson::LoadViaFile(msFilterPath);
        const auto& filter_map = filter_json.GetObject();

        const auto find_ite = filter_map.find(msGameName);
        if (find_ite == filter_map.end()) { throw std::runtime_error("GetFilterSet: not find target game filter obj"); }

        std::unordered_set<std::string> filter_set;
        for (const auto& val : find_ite->second.GetArray())
        {
            filter_set.insert(val.GetStr());
        }

        return filter_set;
    }

    auto Cryptor::BatchDec(const std::string_view msScriptDir, const std::string_view msSaveDir, const std::unordered_set<std::string>& stFilter) -> void
    {
        std::string path_tmp;
        ZxMem tmp_mem(0x64000);
        ZxMem script_mem(0x64000);

        ZxFS::DirMake(msSaveDir, false);

        for (ZxFS::Walk walk{ msScriptDir };walk.NextFile();)
        {
            const auto script_path = walk.GetPath();
            const auto script_name = walk.GetName();

            if (stFilter.find(std::string{ script_name }) != stFilter.end())
            {
                ZxFS::FileCopy(script_path, path_tmp.append(msSaveDir).append(ZxFS::FileSuffixDel(script_name)).append(".cfg"), false);
            }
            else
            {
                script_mem.Load(script_path);
                Cryptor::Dec(script_mem, tmp_mem);
                tmp_mem.Save(path_tmp.append(msSaveDir).append(ZxFS::FileSuffixDel(script_name)).append(".scn"));
            }

            path_tmp.clear();
        }
    }

    auto Cryptor::BatchDec(const std::string_view msScriptPath, const std::string_view msSavePath, const std::string_view msFilterPath, const std::string_view msGameName) -> void
    {
        Cryptor::BatchDec(msScriptPath, msSavePath, Cryptor::GetFilterSet(msFilterPath, msGameName));
    }
}
