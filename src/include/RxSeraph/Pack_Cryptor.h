#pragma once
#include <span>
#include <cstdint>
#include <ZxMem/ZxMem.h>


namespace ZQF::RxSeraph::Pack
{
    class Cryptor
    {
    private:
        ZxMem m_mCache;

    public:
        Cryptor() {};

    public:
        auto Decrypt(ZxMem& amEncBuffer) -> ZxMem&;

    private:
        auto ZLIBDecompress(const std::span<const std::uint8_t> spData) -> void;
    };

}
