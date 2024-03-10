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
		size_t m_nCodePage = 0;
		const std::span<uint8_t> m_spScript;

	public:
		Reader()
		{

		}

		Reader(std::span<uint8_t> spScript, size_t nCodePage) : m_spScript(spScript), m_nCodePage(nCodePage)
		{

		}

		Reader(std::span<uint8_t> spScript, uint32_t nPC, size_t nCodePage) : m_spScript(spScript), m_nPC(nPC), m_nCodePage(nCodePage)
		{

		}

		size_t GetCodePage() const
		{
			return m_nCodePage;
		}

		template<class T> auto Read()
		{
			if constexpr (std::is_same_v<T, std::wstring>)
			{
				char* str_ptr = reinterpret_cast<char*>(m_spScript.data() + m_nPC);
				size_t str_len = strlen(str_ptr);
				m_nPC += str_len + 1;
				return Rut::RxStr::ToWCS({ str_ptr ,str_len }, this->GetCodePage());
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				char* str_ptr = reinterpret_cast<char*>(m_spScript.data() + m_nPC);
				size_t str_len = strlen(str_ptr);
				m_nPC += str_len + 1;
				return std::string{ str_ptr ,str_len };
			}
			else
			{
				T val = *reinterpret_cast<T*>(m_spScript.data() + m_nPC);
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

		uint8_t ReadOP()
		{
			m_ucOP = this->Read<uint8_t>();
			return m_ucOP;
		}

		uint8_t* GetCurPtr() const
		{
			return m_spScript.data() + this->GetPC();
		}

		uint8_t GetCurOP() const
		{
			return m_ucOP;
		}

		size_t GetScriptSize() const
		{
			return m_spScript.size();
		}
	};
}