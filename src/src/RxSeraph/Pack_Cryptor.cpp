#include <RxSeraph/Pack_Cryptor.h>
#include <zlib.h>
#include <ranges>


namespace ZQF::RxSeraph::Pack
{
    auto Cryptor::Decrypt(ZxMem& amEncBuffer) -> ZxMem&
    {
        const std::uint32_t flag0 = amEncBuffer.Get<std::uint32_t>();
        const std::uint8_t flag1 = amEncBuffer.Get<std::uint8_t>();

        if (flag0 == 1 && (flag1 == 0x78))
        {
            const std::uint32_t compressed_size = static_cast<std::uint32_t>(amEncBuffer.SizeBytes() - sizeof(flag0));
            const std::uint8_t* compressed_data_ptr = amEncBuffer.Ptr() + sizeof(flag0);
            this->ZLIBDecompress({ compressed_data_ptr, compressed_size });
            return m_mCache;
        }

        return amEncBuffer;
    }

    auto Cryptor::ZLIBDecompress(const std::span<const std::uint8_t> spData) -> void
    {
        uLong compressed_size = (uLong)spData.size();
        const Bytef* compressed_data_ptr = (Bytef*)spData.data();
        uLongf uncompressed_size = (uLongf)spData.size();
        std::size_t status = Z_FILTERED;
        for ([[maybe_unused]] auto _ : std::views::iota(0, 4))
        {
            uncompressed_size *= 2;
            m_mCache.Resize(uncompressed_size);
            status = ::uncompress(m_mCache.Ptr(), &uncompressed_size, compressed_data_ptr, compressed_size);
            if (status == Z_OK) { break; }
        }

        if (status == Z_FILTERED)
        {
            throw std::runtime_error("zlib decompress failed!");
        }

        m_mCache.Resize(uncompressed_size, false, true);
    }
}
