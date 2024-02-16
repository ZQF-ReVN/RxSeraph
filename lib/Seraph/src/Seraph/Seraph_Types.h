#pragma once
#include <cstdint>


namespace Seraph
{
    struct Dat_Sengmet_HDR
    {
        uint32_t uiSegmentCount;
        uint32_t uiFileCount;
    };

    struct Dat_Segment_Entry
    {
        uint32_t uiSegmentFOA;
        uint32_t uiFileCount;
    };
}