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
		ScenarioParser m_ScenarioParser;
		ExpressionParser m_ExpressionParser;

	public:
		Parser(std::span<uint8_t> spCode) : m_Reader(spCode), m_ScenarioParser(m_Reader), m_ExpressionParser(m_Reader)
		{
			 
		}

	public:
		void ParseCharacterName(Rut::RxJson::JArray& rCodes, std::unordered_map<size_t, std::wstring>& rNameTable)
		{
			uint8_t* param_ptr = m_Reader.GetCurPtr() - 7;
			if (param_ptr[1] == 0x00 && param_ptr[2] == 0x05 && param_ptr[7] == 0xFF)
			{
				uint32_t char_name_inmage_seq = *(uint32_t*)(param_ptr + 3);
				Rut::RxJson::JObject char_name_json;
				char_name_json[L"self"] = L"character";
				char_name_json[L"name"] = rNameTable[char_name_inmage_seq];
				rCodes.emplace_back(std::move(char_name_json));
			}
			else
			{
				throw std::runtime_error("Parser::Parse: not find character name");
			}
		}

		void ParseScenario(Rut::RxJson::JArray& rCodes)
		{
			Rut::RxJson::JObject msg_json;
			msg_json[L"self"] = L"msg";
			msg_json[L"begin"] = (int)m_Reader.GetPC();
			msg_json[L"texts"] = m_ScenarioParser.Parse();
			rCodes.emplace_back(std::move(msg_json));
		}

		void ParseSelect(Rut::RxJson::JArray& rCodes, size_t nSelectCodeSize)
		{
			// get select texts
			uint8_t* text_0_ptr = m_Reader.GetCurPtr() + nSelectCodeSize + 0xA;
			std::string_view text_0 = (char*)text_0_ptr;
			uint8_t* text_1_ptr = text_0_ptr + text_0.size() + 1;
			std::string_view text_1 = (char*)text_1_ptr;

			// gen select json
			Rut::RxJson::JObject select_json;
			select_json[L"self"] = L"select";
			select_json[L"begin"] = (int)m_Reader.GetPC();
			Rut::RxJson::JArray& select_text_jarray = select_json[L"texts"].ToAry();
			select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_0, 932));
			select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_1, 932));

			// add json
			rCodes.emplace_back(std::move(select_json));

			// skip draw select box code
			m_Reader.SkipPC(nSelectCodeSize + 0xA + text_0.size() + text_1.size() + 2 + 1);
		}

		Rut::RxJson::JValue Parse(std::unordered_map<size_t, std::wstring>& rNameTable)
		{
			if (m_Reader.GetCodeSize() <= 31)
			{
				return Rut::RxJson::JValue{};
			}

			constexpr std::array<uint8_t, 31> search_draw_select_box =
			{
				0x56, 0x05, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00,
				0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF
			};

			constexpr std::array<uint8_t, 7> search_draw_text_box =
			{
				0xFF, 0x0F, 0x09, 0x1F, 0x00, 0x00, 0x47
			};

			uint32_t max_search_size = m_Reader.GetCodeSize() - 31;
			Rut::RxJson::JArray codes;
			for (; m_Reader.GetPC() < max_search_size;)
			{
				uint8_t* cur_ptr = m_Reader.GetCurPtr();

				if (memcmp(cur_ptr, search_draw_text_box.data(), search_draw_text_box.size()) == 0)
				{
					this->ParseCharacterName(codes, rNameTable);
					m_Reader.SkipPC(search_draw_text_box.size());
					this->ParseScenario(codes);
				}
				else if (memcmp(cur_ptr, search_draw_select_box.data(), search_draw_select_box.size()) == 0) // parse DrawSelect
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
