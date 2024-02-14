#pragma once
#include <ranges>
#include <vector>
#include <filesystem>
#include <Rut/RxMem.h>
#include <Rut/RxFile.h>
#include <zlib.h>

namespace Seraph
{
	std::wstring NumToStr(const wchar_t* wpFormat, size_t nValue)
	{
		wchar_t buf[0x10];
		size_t len = (size_t)swprintf_s(buf, 0x10, wpFormat, nValue);
		return { buf, len };
	}

	class Dat
	{
	private:
		struct Entry
		{
			uint32_t m_uiFOA;
			uint32_t m_uiSize;
		};

		std::vector<Entry> m_vcIndex;
		std::filesystem::path m_phPack;

	public:
		Dat(const std::filesystem::path& phPack)
		{
			m_phPack = phPack;
		}

		void ReadIndex(uint32_t uiBase = 0)
		{
			Rut::RxFile::Binary ifs_pack{ m_phPack, Rut::RIO_READ };
			uint32_t file_count = ifs_pack.Get<uint32_t>();

			uint32_t first_offset = ifs_pack.Get<uint32_t>() + uiBase;
			uint32_t next_offset = ifs_pack.Get<uint32_t>() + uiBase;
			for (auto ite_entry : std::views::iota(0u, file_count))
			{
				m_vcIndex.emplace_back(first_offset, next_offset - first_offset);
				first_offset = next_offset;
				next_offset = ifs_pack.Get<uint32_t>() + uiBase;
			}
		}

		void Decompress(Rut::RxMem::Auto& rfBuffer, uint8_t* pCompressedData, uint32_t uiCompressedSize)
		{
			uLong compressed_size = (uLong)uiCompressedSize;
			const Bytef* compressed_data_ptr = (Bytef*)pCompressedData;
			uLongf uncompressed_size = (uLongf)uiCompressedSize;
			size_t status = Z_FILTERED;
			for (size_t ite_times = 0; ite_times < 4; ite_times++)
			{
				uncompressed_size *= 2;
				rfBuffer.SetSize(uncompressed_size);
				status = ::uncompress(rfBuffer.GetPtr(), &uncompressed_size, compressed_data_ptr, compressed_size);
				if (status == Z_OK) { break; }
			}

			if (status == Z_FILTERED)
			{
				throw std::runtime_error("zlib decompress failed!");
			}

			rfBuffer.SetSize(uncompressed_size);
		}

		static size_t LZ77Decompress(uint8_t* pRaw, uint8_t* pDec)
		{
			size_t ite_size = 0;
			size_t dec_size = (size_t)(*((uint32_t*)pRaw));
			uint8_t* enc_ptr = pRaw + 4;
			uint8_t* dec_ptr = pDec;
			if (dec_size == 0) { return 0; }

			do
			{
				uint8_t token = *enc_ptr++;
				if (token & 0x80) // expand
				{
					uint32_t expand_info = *enc_ptr++ + (token << 8);
					uint32_t back_offset = (uint16_t)(((expand_info >> 5) & 0x3FF) + 1);
					uint8_t* back_ptr = pDec - back_offset;
					uint32_t copy_size = (expand_info & 0x1F) + 1;
					ite_size += copy_size;

					do
					{
						*pDec++ = *back_ptr++;
					} while (--copy_size);
				}
				else // just copy 
				{
					uint32_t copy_size = (token & 0x7F) + 1;
					ite_size += copy_size;

					do
					{
						*pDec++ = *enc_ptr++;
					} while (--copy_size);
				}

			} while (ite_size < dec_size);

			return ite_size;
		}

		void Unpack()
		{
			Rut::RxFile::Binary ifs_pack{ m_phPack, Rut::RIO_READ };

			std::filesystem::path folder = m_phPack.stem();
			std::filesystem::create_directory(folder);

			Rut::RxMem::Auto buffer(0x64000);
			Rut::RxMem::Auto zlib_buffer(0x64000);

			size_t file_count = m_vcIndex.size();
			size_t ite_seq = 0;
			for (auto& entry : m_vcIndex)
			{
				buffer.ReadData(ifs_pack, entry.m_uiSize, entry.m_uiFOA);
				uint32_t compress_flag = *(buffer.GetPtr<uint32_t*>() + 0);
				uint32_t compressed_size = entry.m_uiSize - sizeof(compress_flag);
				uint8_t* compressed_data_ptr = buffer.GetPtr() + sizeof(compress_flag);
				this->Decompress(zlib_buffer, compressed_data_ptr, compressed_size);

				std::filesystem::path file_save_path = folder / NumToStr(L"%d", ite_seq);

				if (ite_seq > 1 && ite_seq < (file_count -3))
				{
					size_t dec_size = this->LZ77Decompress(zlib_buffer.GetPtr(), buffer.GetPtr());
					buffer.SetSize(dec_size);
					buffer.SaveData(file_save_path);
				}
				else
				{
					zlib_buffer.SaveData(file_save_path);
				}

				ite_seq++;
			}
		}
	};
}