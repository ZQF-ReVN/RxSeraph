#include <RxSeraph/Pack.h>
#include <RxSeraph/Pack_Cryptor.h>
#include <RxSeraph/Pack_IndexTable.h>
#include <ZxFile/ZxFile.h>
#include <ranges>


namespace ZQF::RxSeraph::Pack
{
    auto GuessFileType(const std::uint16_t nSignature) -> std::string_view
    {
        switch (nSignature)
        {
        case 0x5843: return ".cx"; break; // 'CX'
        case 0x4643: return ".cf"; break; // 'CF'
        case 0x5443: return ".ct"; break; // 'CT'
        case 0x4343: return ".cc"; break; // 'CC'
        case 0x4243: return ".cb"; break; // 'CB'
        case 0x4D42: return ".bmp"; break; // 'BM'
        default: return ".unknow";
        }
    }

    auto Export(const std::string_view msPackPath, const std::string_view msSaveDir, const uint32_t uiIndexOffset) -> void
    {
        Pack::IndexTable index_table(msPackPath, uiIndexOffset);

        ZxFile ifs_pack{ msPackPath, ZxFile::OpenMod::ReadSafe };

        ZxMem cache_mem;
        Pack::Cryptor cryptor;
        for (auto [idx, entry] : std::views::enumerate(index_table.GetVector()))
        {
            if (entry.nSize == 0) { continue; }

            ifs_pack.SetPtr(entry.nFOA, ZxFile::MoveWay::Beg);
            ifs_pack.Read(cache_mem.Resize(entry.nSize).Span());

            ZxMem& dec_mem = cryptor.Decrypt(cache_mem);

            std::string save_path;
            {
                std::string_view suffix_str = Pack::GuessFileType(dec_mem.Get<std::uint16_t>());
                save_path = std::format("{}/{:#08}{}", msSaveDir, idx, suffix_str);
            }

            dec_mem.Save(save_path);
        }
    }
 
}
