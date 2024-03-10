#pragma once
#include <span>
#include <array>
#include <memory>
#include <ranges>

#include <Rut/RxMem.h>
#include <Rut/RxStr.h>
#include <Rut/RxJson.h>

#include "Script_Scenario.h"
#include "Script_Expression.h"


namespace Seraph::Script::V2
{
	class Parser
	{
	private:
		Reader m_Reader;
		Scenario m_Scenario;
		Expression m_Expression;

	public:
		Parser(std::span<uint8_t> spCode, size_t nCodePage) : m_Reader(spCode, nCodePage), m_Scenario(m_Reader), m_Expression(m_Reader)
		{
			 
		}

	public:
		void ParseCharName(Rut::RxJson::JArray& jaCodes, std::vector<std::wstring>& vcNameTable, size_t nCharNameCodeSize)
		{
			uint8_t* param_ptr = m_Reader.GetCurPtr() - 7;

			if (param_ptr[1] != 0x00 || param_ptr[2] != 0x05 || param_ptr[7] != 0xFF)
			{
				throw std::runtime_error("Parser::ParseCharacterName: not find character name");
			}

			uint32_t char_name_inmage_seq = *(uint32_t*)(param_ptr + 3);
			Rut::RxJson::JObject char_name_json;
			char_name_json[L"self"] = L"character";
			char_name_json[L"name"] = vcNameTable[char_name_inmage_seq];
			jaCodes.emplace_back(std::move(char_name_json));
			m_Reader.SkipPC(nCharNameCodeSize);
		}

		void ParseScenario(Rut::RxJson::JArray& jaCodes)
		{
			Rut::RxJson::JObject scn_json;
			scn_json[L"self"] = L"msg";
			scn_json[L"begin"] = (int)m_Reader.GetPC();
			scn_json[L"texts"] = m_Scenario.Parse();
			jaCodes.emplace_back(std::move(scn_json));
		}

		void ParseSelect(Rut::RxJson::JArray& jaCodes, size_t nSelectCodeSize)
		{
			uint8_t* text_0_ptr = m_Reader.GetCurPtr() + nSelectCodeSize + 0xA;
			std::string_view text_0 = (char*)text_0_ptr;
			uint8_t* text_1_ptr = text_0_ptr + text_0.size() + 1;
			std::string_view text_1 = (char*)text_1_ptr;

			Rut::RxJson::JObject select_json;
			select_json[L"self"] = L"select";
			select_json[L"begin"] = (int)m_Reader.GetPC();
			Rut::RxJson::JArray& select_text_jarray = select_json[L"texts"].ToAry();
			select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_0, m_Reader.GetCodePage()));
			select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_1, m_Reader.GetCodePage()));

			jaCodes.emplace_back(std::move(select_json));

			m_Reader.SkipPC(nSelectCodeSize + 0xA + text_0.size() + text_1.size() + 2 + 1);
		}

		Rut::RxJson::JValue Parse(std::vector<std::wstring>& vcNameTable)
		{
			if (m_Reader.GetScriptSize() <= 31) { return Rut::RxJson::JValue{}; }

			constexpr std::array<uint8_t, 31> search_draw_select_box =
			{
				0x56, 0x05, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00,
				0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF
			};

			constexpr std::array<uint8_t, 7> search_draw_text_box =
			{
				0xFF, 0x0F, 0x09, 0x1F, 0x00, 0x00, 0x47
			};

			uint32_t max_search_size = m_Reader.GetScriptSize() - 31;
			Rut::RxJson::JArray codes;
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

	};
}
