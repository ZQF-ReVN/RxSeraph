#include <RxSeraph/Core/Script.h>
#include <array>


namespace ZQF::RxSeraph::Script::V2
{
    Parser::Parser(std::span<std::uint8_t> spCode, std::size_t nCodePage) : m_Reader(spCode, nCodePage), m_Scenario(m_Reader), m_Expression(m_Reader)
    {

    }

    auto Parser::ParseCharName(ZxJson::JArray_t& jaCodes, std::vector<std::string>& vcNameTable, const std::size_t nCharNameCodeSize) -> void
    {
        std::uint8_t* param_ptr = m_Reader.GetCurPtr() - 7;

        if (param_ptr[1] != 0x00 || param_ptr[2] != 0x05 || param_ptr[7] != 0xFF)
        {
            throw std::runtime_error("Parser::ParseCharacterName: not find character name");
        }

        const auto char_name_inmage_seq = *reinterpret_cast<std::uint32_t*>(param_ptr + 3);

        jaCodes.emplace_back
        (
            ZxJson::JObject_t
            {
                { "self", "character" },
                { "name", vcNameTable[char_name_inmage_seq] }
            }
        );

        m_Reader.SkipPC(static_cast<std::uint32_t>(nCharNameCodeSize));
    }

    auto Parser::ParseScenario(ZxJson::JArray_t& jaCodes) -> void
    {
        jaCodes.emplace_back
        (
            ZxJson::JObject_t
            {
                { "self", "msg" },
                { "begin", m_Reader.GetPC() },
                { "texts", m_Scenario.Parse() }
            }
        );
    }

    auto Parser::ParseSelect(ZxJson::JArray_t& jaCodes, const std::size_t nSelectCodeSize) -> void
    {
        std::uint8_t* text_0_ptr = m_Reader.GetCurPtr() + nSelectCodeSize + 0xA;
        const std::string_view text_0{ reinterpret_cast<char*>(text_0_ptr) };
        std::uint8_t* text_1_ptr = text_0_ptr + text_0.size() + 1;
        const std::string_view text_1{ reinterpret_cast<char*>(text_1_ptr) };

        jaCodes.emplace_back
        (
            ZxJson::JObject_t
            {
                { "self", "select" },
                { "begin", m_Reader.GetPC() },
                { "texts", ZxJson::JArray_t{ m_Cvt.MBCSToUTF8(text_0, m_Reader.GetCodePage()), m_Cvt.MBCSToUTF8(text_1, m_Reader.GetCodePage()) } }
            }
        );

        m_Reader.SkipPC(static_cast<std::uint32_t>(nSelectCodeSize + 0xA + text_0.size() + text_1.size() + 2 + 1));
    }

    auto Parser::Parse(std::vector<std::string>& vcNameTable) -> ZxJson::JArray_t
    {
        if (m_Reader.GetScriptSize() <= 31) { return ZxJson::JArray_t{}; }

        constexpr std::array<std::uint8_t, 31> search_draw_select_box =
        {
            0x56, 0x05, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00,
            0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF
        };

        constexpr std::array<std::uint8_t, 7> search_draw_text_box =
        {
            0xFF, 0x0F, 0x09, 0x1F, 0x00, 0x00, 0x47
        };

        ZxJson::JArray_t codes;
        const auto max_search_size = static_cast<std::uint32_t>(m_Reader.GetScriptSize() - 31);
        for (; m_Reader.GetPC() < max_search_size;)
        {
            if (memcmp(m_Reader.GetCurPtr(), search_draw_text_box.data(), search_draw_text_box.size()) == 0)
            {
                this->ParseCharName(codes, vcNameTable, search_draw_text_box.size());
                this->ParseScenario(codes);
            }
            else if (memcmp(m_Reader.GetCurPtr(), search_draw_select_box.data(), search_draw_select_box.size()) == 0)
            {
                this->ParseSelect(codes, search_draw_select_box.size());
            }
            else
            {
                m_Reader.SkipPC(1);
            }
        }

        return codes;
    }
}
