#pragma once
#include <cstdint>
#include <Rut/RxMem.h>


namespace Seraph::Script
{
	class Crypt
	{
	public:
		Crypt()
		{

		}

		static size_t LZ77Dec(uint8_t* pRaw, uint8_t* pDec)
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

		static bool Dec(Rut::RxMem::Auto& rScript, Rut::RxMem::Auto& rDec, size_t nMaxDecSize = 0x64000)
		{
			rDec.SetSize(nMaxDecSize);
			size_t size = Crypt::LZ77Dec(rScript.GetPtr(), rDec.GetPtr());
			rDec.SetSize(size);
			return size ? true : false;
		}
	};

}