#pragma once
#include <cstdint>


namespace ZQF::RxSeraph
{
    struct Dat_Sengmet_HDR
    {
        std::uint32_t nSegmentCnt;
        std::uint32_t nFileCnt;

        static consteval auto Sizebyte() -> std::size_t
        {
            return sizeof(nSegmentCnt) + sizeof(nFileCnt);
        }
    };

    struct Dat_Segment_Entry
    {
        uint32_t nSegmentFOA;
        uint32_t nFileCnt;

        static consteval auto Sizebyte() -> std::size_t
        {
            return sizeof(nSegmentFOA) + sizeof(nFileCnt);
        }
    };

    // typedef void(__stdcall* Fn_ScriptLoad)();
}
