#pragma once
#include <span>
#include <string>
#include <Rut/RxStr.h>


namespace Seraph::Script::V2
{
	class Reader
	{
	private:
		uint8_t m_ucOP = 0;
		uint32_t m_nPC = 0;
		const std::span<uint8_t> m_spCode;

	public:
		Reader()
		{

		}

		Reader(std::span<uint8_t> spCode) : m_spCode(spCode)
		{

		}

		Reader(std::span<uint8_t> spCode, uint32_t nPC) : m_spCode(spCode), m_nPC(nPC)
		{

		}


		template<class T> auto Read()
		{
			if constexpr (std::is_same_v<T, std::wstring>)
			{
				char* str_ptr = (char*)(m_spCode.data() + m_nPC);
				size_t str_len = strlen(str_ptr);
				m_nPC += str_len + 1;
				return Rut::RxStr::ToWCS({ str_ptr ,str_len }, 932);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				char* str_ptr = (char*)(m_spCode.data() + m_nPC);
				size_t str_len = strlen(str_ptr);
				m_nPC += str_len + 1;
				return std::string{ str_ptr ,str_len };
			}
			else
			{
				T val = *(T*)(m_spCode.data() + m_nPC);
				m_nPC += sizeof(T);
				return val;
			}
		}

		void SetPC(uint32_t uiPC)
		{
			m_nPC = uiPC;
		}

		uint32_t GetPC() const
		{
			return m_nPC;
		}

		void SkipPC(uint32_t uiPC)
		{
			m_nPC += uiPC;
		}

		bool NextInstr()
		{
			if (m_ucOP != 0xFF)
			{
				m_ucOP = this->Read<uint8_t>();
				return true;
			}
			else
			{
				m_ucOP = this->Read<uint8_t>();
				return false;
			}
		}

		uint8_t* GetCurPtr() const
		{
			return m_spCode.data() + this->GetPC();
		}

		uint8_t GetOPCode() const
		{
			return m_ucOP;
		}

		size_t GetCodeSize()
		{
			return m_spCode.size();
		}
	};
}