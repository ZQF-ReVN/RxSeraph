#pragma once
#include <cstdint>
#include <string_view>


namespace ZQF::RxSeraph::Pack
{
    auto GuessFileType(const std::uint16_t nSignature) -> std::string_view;
    auto Export(const std::string_view msPackPath, const std::string_view msSaveDir, const uint32_t uiIndexOffset = 0) -> void;
}
