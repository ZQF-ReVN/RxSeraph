#pragma once
#include <cstdint>
#include <unordered_set>
#include <ZxMem/ZxMem.h>


namespace ZQF::RxSeraph::Script
{
    class Cryptor
    {
    public:
        Cryptor();
    private:
        static auto LZ77Dec(std::uint8_t* pRaw, std::uint8_t* pDec) -> std::size_t;

    public:
        static auto Dec(ZxMem& rScript, ZxMem& rDec, const std::size_t nMaxDecSize = 0x64000) -> bool;

    private:
        static auto GetFilterSet(const std::string_view msFilterPath, const std::string_view msGameName) -> std::unordered_set<std::string>;

    public:
        static auto BatchDec(const std::string_view msScriptPath, const std::string_view msSavePath, const std::unordered_set<std::string>& stFilter) -> void;
        static void BatchDec(const std::string_view msScriptPath, const std::string_view msSavePath, const std::string_view msFilterPath, const std::string_view msGameName);
    };

}
