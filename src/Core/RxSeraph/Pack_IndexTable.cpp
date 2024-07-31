#include <RxSeraph/Pack_IndexTable.h>
#include <RxSeraph/Seraph_Types.h>
#include <ZxFile/ZxFile.h>
#include <ranges>


namespace ZQF::RxSeraph::Pack
{
    IndexTable::IndexTable()
    {

    }

    IndexTable::IndexTable(const std::string_view msPackPath, const std::uint32_t uiIndexOffset)
    {
        this->ReadIndex(msPackPath, uiIndexOffset);
    }

    auto IndexTable::ReadIndexBased(ZxMem& rfIndexMem, const std::uint32_t uiDataBegOffset, const std::uint32_t uiFileCount) -> void
    {
        auto first_offset = rfIndexMem.Get<std::uint32_t>() + uiDataBegOffset;
        auto next_offset = rfIndexMem.Get<std::uint32_t>() + uiDataBegOffset;
        for ([[maybe_unused]] auto _ : std::views::iota(0u, uiFileCount))
        {
            m_vcIndex.emplace_back(IndexTable::Entry{ .nFOA = first_offset, .nSize = next_offset - first_offset });
            first_offset = next_offset;
            next_offset = rfIndexMem.Get<std::uint32_t>() + uiDataBegOffset;
        }
    }

    auto IndexTable::ReadGenericIndex(const std::string_view msPackPath) -> void
    {
        ZxFile ifs_pack{ msPackPath, ZxFile::OpenMod::ReadSafe };
        const auto file_count = ifs_pack.Get<uint32_t>();
        ZxMem index_mem((file_count + 2) * sizeof(std::uint32_t));
        ifs_pack.Read(index_mem.Span());
        this->ReadIndexBased(index_mem, 0, file_count);
    }

    auto IndexTable::ReadSegmentIndex(const std::string_view msPackPath, const std::uint32_t uiIndexOffset) -> void
    {
        ZxFile ifs_pack{ msPackPath, ZxFile::OpenMod::ReadSafe };
        ifs_pack.SetPtr(uiIndexOffset, ZxFile::MoveWay::Beg);

        Dat_Sengmet_HDR hdr{ .nSegmentCnt = ifs_pack.Get<std::uint32_t>(), .nFileCnt = ifs_pack.Get<std::uint32_t>() };
        m_vcIndex.reserve(hdr.nFileCnt);

        ZxMem cache_mem;

        ZxMem& seg_index_table_mem = cache_mem;
        std::vector<Dat_Segment_Entry> seg_entry_vec;
        {
            const auto seg_index_table_mem_bytes = static_cast<std::size_t>(hdr.nSegmentCnt) * Dat_Sengmet_HDR::Sizebyte();
            ifs_pack.Read(seg_index_table_mem.Resize(seg_index_table_mem_bytes).Span());

            for ([[maybe_unused]] auto _ : std::views::iota(0u, hdr.nSegmentCnt))
            {
                seg_entry_vec.emplace_back
                (
                    Dat_Segment_Entry
                    {
                    .nSegmentFOA = seg_index_table_mem.Get<std::uint32_t>(),
                    .nFileCnt = seg_index_table_mem.Get<std::uint32_t>()
                    }
                );
            }
        }

        ZxMem& index_mem = cache_mem;
        {
            for (auto ite_seg_entry = std::rbegin(seg_entry_vec); ite_seg_entry != std::rend(seg_entry_vec); ite_seg_entry++)
            {
                Dat_Segment_Entry& seg_entry = *ite_seg_entry;
                const auto index_table_bytes = static_cast<std::size_t>(seg_entry.nFileCnt + 2) * sizeof(std::uint32_t);
                ifs_pack.Read(index_mem.Resize(index_table_bytes).Span());
                this->ReadIndexBased(index_mem, seg_entry.nSegmentFOA, seg_entry.nFileCnt);
                ifs_pack.SetPtr(static_cast<std::uint64_t>(-4), ZxFile::MoveWay::Cur);
            }
        }
    }

    auto IndexTable::ReadIndex(const std::string_view msPackPath, const std::uint32_t uiIndexOffset) -> void
    {
        if (uiIndexOffset == 0)
        {
            this->ReadGenericIndex(msPackPath);
        }
        else
        {
            this->ReadSegmentIndex(msPackPath, uiIndexOffset);
        }
    }

    auto IndexTable::GetVector() const -> const std::vector<Entry>&
    {
        return m_vcIndex;
    }
}
