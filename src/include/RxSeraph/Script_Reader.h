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
        std::uint8_t m_ucOP{};
        std::uint32_t m_nPC{};
        std::size_t m_nCodePage{};
		const std::span<std::uint8_t> m_spScript;

	public:
		Reader()
		{

		}

		Reader(const std::span<uint8_t> spScript, const std::size_t nCodePage) : m_nCodePage(nCodePage), m_spScript(spScript)
		{

		}

		Reader(const std::span<uint8_t> spScript, const std::uint32_t nPC, const std::size_t nCodePage) : m_nPC(nPC), m_nCodePage(nCodePage), m_spScript(spScript)
		{

		}

        std::size_t GetCodePage() const
		{
			return m_nCodePage;
		}

		template<class T> auto Read()
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				char* str_ptr = reinterpret_cast<char*>(m_spScript.data() + m_nPC);
                std::string_view str = str_ptr;
				m_nPC += static_cast<std::uint32_t>(str.size() + 1);
                return std::string{ ZxCvt{}.MBCSToUTF8(str, m_nCodePage) };
			}
			else
			{
				T val = *reinterpret_cast<T*>(m_spScript.data() + m_nPC);
				m_nPC += sizeof(T);
				return val;
			}
		}

		void SetPC(std::uint32_t uiPC)
		{
			m_nPC = uiPC;
		}

        std::uint32_t GetPC() const
		{
			return m_nPC;
		}

		void SkipPC(std::uint32_t uiPC)
		{
			m_nPC += uiPC;
		}

        std::uint8_t ReadOP()
		{
			m_ucOP = this->Read<std::uint8_t>();
			return m_ucOP;
		}

        std::uint8_t* GetCurPtr() const
		{
			return m_spScript.data() + this->GetPC();
		}

        std::uint8_t GetCurOP() const
		{
			return m_ucOP;
		}

        std::size_t GetScriptSize() const
		{
			return m_spScript.size();
		}
	};
}
