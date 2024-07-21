#pragma once
#include <span>
#include <string>
#include <string_view>
#include <ZxCvt/ZxCvt.h>


namespace ZQF::RxSeraph::Script::V2
{
	class Reader
	{
	private:
        uint8_t m_ucOP{};
        uint32_t m_nPC{};
        size_t m_nCodePage{};
		const std::span<uint8_t> m_spScript;

	public:
		Reader()
		{

		}

		Reader(std::span<uint8_t> spScript, size_t nCodePage) : m_nCodePage(nCodePage), m_spScript(spScript)
		{

		}

		Reader(std::span<uint8_t> spScript, uint32_t nPC, size_t nCodePage) : m_nPC(nPC), m_nCodePage(nCodePage), m_spScript(spScript)
		{

		}

		size_t GetCodePage() const
		{
			return m_nCodePage;
		}

		template<class T> auto Read()
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				char* str_ptr = reinterpret_cast<char*>(m_spScript.data() + m_nPC);
                std::string_view str = str_ptr;
				m_nPC += static_cast<uint32_t>(str.size() + 1);
                return std::string{ ZxCvt{}.MBCSToUTF8(str, m_nCodePage) };
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
