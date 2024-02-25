#pragma once
#include <span>
#include <ranges>
#include <vector>
#include <filesystem>
#include <Rut/RxMem.h>
#include <Rut/RxFile.h>
#include <zlib.h>
#include "Seraph_Types.h"


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

	public:
		Dat()
		{

		}


		void ReadIndexBased(Rut::RxFile::Binary& ifsPack, uint32_t uiDataBegOffset, uint32_t uiFileCount)
		{
			uint32_t first_offset = ifsPack.Get<uint32_t>() + uiDataBegOffset;
			uint32_t next_offset = ifsPack.Get<uint32_t>() + uiDataBegOffset;
			for (auto ite_entry : std::views::iota(0u, uiFileCount))
			{
				m_vcIndex.emplace_back(first_offset, next_offset - first_offset);
				first_offset = next_offset;
				next_offset = ifsPack.Get<uint32_t>() + uiDataBegOffset;
			}
		}

		void ReadGenericIndex(const std::filesystem::path& phPack)
		{
			Rut::RxFile::Binary ifs_pack{ phPack, Rut::RIO_READ };
			uint32_t file_count = ifs_pack.Get<uint32_t>();
			this->ReadIndexBased(ifs_pack, 0, file_count);
		}

		void ReadSegmentIndex(const std::filesystem::path& phPack, uint32_t uiIndexOffset)
		{
			Rut::RxFile::Binary ifs_pack{ phPack, Rut::RIO_READ };
			ifs_pack.SetPos(uiIndexOffset);

			Dat_Sengmet_HDR hdr = ifs_pack.Get<Dat_Sengmet_HDR>();
			m_vcIndex.reserve(hdr.uiFileCount);

			std::vector<Dat_Segment_Entry> seg_entry_vec;
			for (auto ite_seg_entry : std::views::iota(0u, hdr.uiSegmentCount))
			{
				seg_entry_vec.emplace_back(ifs_pack.Get<Dat_Segment_Entry>());
			}

			
			for (auto ite_seg_entry = std::rbegin(seg_entry_vec); ite_seg_entry != std::rend(seg_entry_vec); ite_seg_entry++)
			{
				Dat_Segment_Entry& seg_entry = *ite_seg_entry;
				this->ReadIndexBased(ifs_pack, seg_entry.uiSegmentFOA, seg_entry.uiFileCount);
				ifs_pack.SetPos(-4, Rut::RIO_CURRENT);
			}
		}

		void ReadIndex(const std::filesystem::path& phPack, uint32_t uiIndexOffset = 0)
		{
			if (uiIndexOffset == 0)
			{
				this->ReadGenericIndex(phPack);
			}
			else
			{
				this->ReadSegmentIndex(phPack, uiIndexOffset);
			}
		}


		size_t LZ77Decompress(uint8_t* pRaw, uint8_t* pDec)
		{
			size_t ite_size = 0;
			size_t dec_size = (size_t)(*((uint32_t*)pRaw));

			uint8_t* dec_ptr = pDec;
			uint8_t* enc_ptr = pRaw + 4;

			if (dec_size == 0) { return 0; }

			do
			{
				uint8_t token = *enc_ptr++;
				if (token & 0x80) // expand
				{
					uint32_t expand_info = *enc_ptr++ + (token << 8);
					
					uint32_t copy_size = (expand_info & 0x1F) + 1;
					ite_size += copy_size;

					uint32_t back_offset = (uint16_t)(((expand_info >> 5) & 0x3FF) + 1);
					uint8_t* back_ptr = pDec - back_offset;

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

		void ZLIBDecompress(Rut::RxMem::Auto& amBuffer, std::span<uint8_t> spData)
		{
			uLong compressed_size = (uLong)spData.size();
			const Bytef* compressed_data_ptr = (Bytef*)spData.data();
			uLongf uncompressed_size = (uLongf)spData.size();
			size_t status = Z_FILTERED;
			for (auto ite_times : std::views::iota(0, 4))
			{
				uncompressed_size *= 2;
				amBuffer.SetSize(uncompressed_size);
				status = ::uncompress(amBuffer.GetPtr(), &uncompressed_size, compressed_data_ptr, compressed_size);
				if (status == Z_OK) { break; }
			}

			if (status == Z_FILTERED)
			{
				throw std::runtime_error("zlib decompress failed!");
			}

			amBuffer.SetSize(uncompressed_size);
		}

		void Decrypt(Rut::RxMem::Auto& amEncBuffer, Rut::RxMem::Auto& amDecBuffer)
		{
			uint32_t flag = *(amEncBuffer.GetPtr<uint32_t*>() + 0);

			if (flag == 1)
			{
				uint32_t compressed_size = amEncBuffer.GetSize() - sizeof(flag);
				uint8_t* compressed_data_ptr = amEncBuffer.GetPtr() + sizeof(flag);
				this->ZLIBDecompress(amDecBuffer, { compressed_data_ptr, compressed_size });
			}
			else
			{
				throw std::runtime_error("Seraph::Dat::Decrypt: Unknow Format!");
			}
		}


		std::wstring GuessFileType(Rut::RxMem::Auto& amFile)
		{
			uint16_t signature = *amFile.GetPtr<uint16_t*>();
			switch (signature)
			{
			case 0x5843: return L".cx"; break; // 'CX'
			case 0x4643: return L".cf"; break; // 'CF'
			case 0x5443: return L".ct"; break; // 'CT'
			case 0x4343: return L".cc"; break; // 'CC'
			case 0x4243: return L".cb"; break; // 'CB'
			case 0x4D42: return L".bmp"; break; // 'BM'
			default: return L".unknow";
			}
		}

		void UnpackScnPac(const std::filesystem::path& phPack)
		{
			Rut::RxFile::Binary ifs_pack{ phPack, Rut::RIO_READ };
			this->ReadIndex(phPack);

			std::filesystem::path folder = phPack.stem();
			std::filesystem::create_directory(folder);

			Rut::RxMem::Auto raw_buffer(0x64000);
			Rut::RxMem::Auto dec_buffer(0x64000);

			size_t file_count = m_vcIndex.size();
			for (auto [seq, entry] : std::views::enumerate(m_vcIndex))
			{
				raw_buffer.ReadData(ifs_pack, entry.m_uiSize, entry.m_uiFOA);
				this->Decrypt(raw_buffer, dec_buffer);

				std::filesystem::path file_save_path = folder / NumToStr(L"%d", seq);

				if (seq > 1 && seq < (file_count -3))
				{
					size_t dec_size = this->LZ77Decompress(dec_buffer.GetPtr(), raw_buffer.GetPtr());
					raw_buffer.SetSize(dec_size);
					raw_buffer.SaveData(file_save_path);
				}
				else
				{
					dec_buffer.SaveData(file_save_path);
				}
			}
		}

		void Unpack(const std::filesystem::path& phPack, uint32_t uiIndexOffset = 0)
		{
			this->ReadIndex(phPack, uiIndexOffset);

			std::filesystem::path folder = phPack.stem();
			std::filesystem::create_directory(folder);

			Rut::RxFile::Binary ifs_pack{ phPack, Rut::RIO_READ };
			
			Rut::RxMem::Auto raw_buffer(0x64000);
			Rut::RxMem::Auto dec_buffer(0x64000);
			for (auto [seq, entry] : std::views::enumerate(m_vcIndex))
			{
				if (entry.m_uiSize == 0) { continue; }
				raw_buffer.ReadData(ifs_pack, entry.m_uiSize, entry.m_uiFOA);
				this->Decrypt(raw_buffer, dec_buffer);
				dec_buffer.SaveData(folder / (NumToStr(L"0x%08x", seq) + this->GuessFileType(dec_buffer)));
			}
		}
	};
}