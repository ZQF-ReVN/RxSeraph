#pragma once
#include <span>
#include <memory>
#include <variant>

#include <Rut/RxStr.h>
#include <Rut/RxJson.h>


namespace Seraph::Script
{
	enum ScenarioInstruction
	{
		Scenario_Text_Push,
		Scenario_Text_Format,
		Scenario_Font_Set_Color_R,
		Scenario_Font_Set_Color_G,
		Scenario_Font_Set_Color_B,
		Scenario_Font_Set_Width,
		Scenario_Font_Set_Heigh,
		Scenario_Un7,
		Scenario_Time_Wait,
		Scenario_Text_Allow_Color_Green,
		Scenario_Text_Draw_Layer,
		Scenario_Text_Allow_Vertical,
		Scenario_Text_Indent_Add = 0xE,
		Scenario_Se_Play,
		Scenario_Cursor_Allow_Click,
		Scenario_Text_Indent_At,
		Scenario_Next_Line = 0x14,
		Scenario_Input_Wait,
		Scenario_Text_Indent_Flag,
		Scenario_Next_Page,
		Scenario_Voice_Play,
		Scenario_Screen_Shake,
		Scenario_End = 0xFF
	};


	class ScenarioParser
	{
	private:
		uint8_t m_ucOP = 0;
		uint32_t m_nPC = 0;
		const std::span<uint8_t> m_spCode;

	public:
		ScenarioParser(std::span<uint8_t> spCode);

	private:
		template<class T> auto Read();

	private:
		bool NextInstr();
		uint8_t GetOPCode() const;

	private:
		std::wstring ParseInstrName() const;
		Rut::RxJson::JObject ParseInstrParam();

	public:
		Rut::RxJson::JValue Parse();
	};

	ScenarioParser::ScenarioParser(std::span<uint8_t> spCode) : m_spCode(spCode)
	{

	}

	template<class T> auto ScenarioParser::Read()
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
			return { str_ptr ,str_len };
		}
		else
		{
			T val = *(T*)(m_spCode.data() + m_nPC);
			m_nPC += sizeof(T);
			return val;
		}
	}

	bool ScenarioParser::NextInstr()
	{
		m_ucOP = this->Read<uint8_t>();
		return m_ucOP != Scenario_End ? true : false;
	}

	uint8_t ScenarioParser::GetOPCode() const
	{
		return m_ucOP;
	}
	
	std::wstring ScenarioParser::ParseInstrName() const
	{
		switch (this->GetOPCode())
		{
		case Scenario_Text_Push: return L"Scenario_Text_Push";
		case Scenario_Text_Format: return L"Scenario_Text_Format";
		case Scenario_Font_Set_Color_R: return L"Scenario_Font_Set_Color_R";
		case Scenario_Font_Set_Color_G: return L"Scenario_Font_Set_Color_G";
		case Scenario_Font_Set_Color_B: return L"Scenario_Font_Set_Color_B";
		case Scenario_Font_Set_Width: return L"Scenario_Font_Set_Width";
		case Scenario_Font_Set_Heigh: return L"Scenario_Font_Set_Heigh";
		case Scenario_Un7: return L"Scenario_Un7";
		case Scenario_Time_Wait: return L"Scenario_Time_Wait";
		case Scenario_Text_Allow_Color_Green: return L"Scenario_Text_Allow_Color_Green";
		case Scenario_Text_Draw_Layer: return L"Scenario_Text_Draw_Layer";
		case Scenario_Text_Allow_Vertical: return L"Scenario_Text_Allow_Vertical";
		case Scenario_Text_Indent_Add: return L"Scenario_Text_Indent_Add";
		case Scenario_Se_Play: return L"Scenario_Se_Play";
		case Scenario_Cursor_Allow_Click: return L"Scenario_Cursor_Allow_Click";
		case Scenario_Text_Indent_At: return L"Scenario_Text_Indent_At";
		case Scenario_Next_Line: return L"Scenario_Next_Line";
		case Scenario_Input_Wait: return L"Scenario_Input_Wait";
		case Scenario_Text_Indent_Flag: return L"Scenario_Text_Indent_Flag";
		case Scenario_Next_Page: return L"Scenario_Next_Page";
		case Scenario_Voice_Play: return L"Scenario_Voice_Play";
		case Scenario_Screen_Shake: return L"Scenario_Screen_Shake";
		case Scenario_End: return L"Scenario_End";
		default: throw std::runtime_error("ScenarioInstr::GetInstrName: Error Command!");
		}
	}

	Rut::RxJson::JObject ScenarioParser::ParseInstrParam()
	{
		Rut::RxJson::JObject param;

		switch (this->GetOPCode())
		{
		case Scenario_Text_Push:
		{
			param[L"type"] = L"Str";
			param[L"value"] = this->Read<std::wstring>();
		}
		break;

		case Scenario_Text_Format:
		{
			throw std::runtime_error("Unknow OPCode!");
		}
		break;

		case Scenario_Font_Set_Color_R:
		case Scenario_Font_Set_Color_G:
		case Scenario_Font_Set_Color_B:
		case Scenario_Font_Set_Width:
		case Scenario_Font_Set_Heigh:
		case Scenario_Un7:
		case Scenario_Time_Wait:
		case Scenario_Text_Allow_Color_Green:
		case Scenario_Text_Draw_Layer:
		case Scenario_Text_Allow_Vertical:
		case Scenario_Text_Indent_Add:
		case Scenario_Se_Play:
		case Scenario_Cursor_Allow_Click:
		case Scenario_Text_Indent_At:
		case Scenario_Screen_Shake:
		{
			param[L"type"] = L"Byte";
			param[L"value"] = this->Read<uint8_t>();
		}
		break;

		case Scenario_Next_Line:
		case Scenario_Input_Wait:
		case Scenario_Text_Indent_Flag:
		case Scenario_Next_Page:
			break;

		case Scenario_Voice_Play:
		{
			param[L"type"] = L"Byte";
			param[L"value"] = this->Read<int>();
		}
		break;

		default: throw std::runtime_error("Unknow OPCode!");
		}

		return param;
	}

	Rut::RxJson::JValue ScenarioParser::Parse()
	{
		Rut::RxJson::JArray codes;

		while (this->NextInstr())
		{
			Rut::RxJson::JObject instr;
			instr[L"opcode"] = this->GetOPCode();
			instr[L"command"] = this->ParseInstrName();
			instr[L"parameter"] = this->ParseInstrParam();
			codes.emplace_back(std::move(instr));
		}

		return codes;
	}
}
