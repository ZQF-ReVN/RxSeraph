#pragma once
#include <vector>
#include <cstdint>
#include <string_view>
#include <ZxMem/ZxMem.h>


namespace ZQF::RxSeraph::Pack
{
    class IndexTable
    {
    private:
        struct Entry
        {
            std::uint32_t nFOA;
            std::uint32_t nSize;
        };

        std::vector<IndexTable::Entry> m_vcIndex;

    public:
        IndexTable();
        IndexTable(const std::string_view msPackPath, const uint32_t uiIndexOffset = 0);

    private:
        auto ReadIndexBased(ZxMem& rfIndexMem, const std::uint32_t uiDataBegOffset, const std::uint32_t uiFileCount) -> void;
        auto ReadGenericIndex(const std::string_view msPackPath) -> void;
        auto ReadSegmentIndex(const std::string_view msPackPath, const std::uint32_t uiIndexOffset) -> void;

    public:
        auto ReadIndex(const std::string_view msPackPath, const std::uint32_t uiIndexOffset = 0) -> void;

    public:
        auto GetVector() const -> const std::vector<Entry>&;
    };
}
