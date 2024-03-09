#pragma once
#include <stdexcept>
#include <Rut/RxJson.h>

#include "Script_Reader.h"
#include "Script_Instruction.h"


namespace Seraph::Script::V2
{
	class ScenarioParser
	{
	private:
		Reader& m_Reader;

	public:
		ScenarioParser(Reader& rReader) : m_Reader(rReader)
		{

		}

		std::wstring ParseScenarioInstrName() const
		{
			switch (m_Reader.GetOPCode())
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
			default: throw std::runtime_error("Parser::ParseScenarioInstrName: Error Command!");
			}
		}

		Rut::RxJson::JValue ParseScenarioInstrParam()
		{
			Rut::RxJson::JObject param;

			switch (m_Reader.GetOPCode())
			{
			case Scenario_Text_Push:
			{
				param[L"value"] = m_Reader.Read<std::wstring>();
			}
			break;

			case Scenario_Text_Format:
			{
				throw std::runtime_error("Parser::ParseScenarioInstrParam: Error!");
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
				param[L"value"] = m_Reader.Read<uint8_t>();
			}
			break;

			case Scenario_Next_Line:
			case Scenario_Input_Wait:
			case Scenario_Text_Indent_Flag:
			case Scenario_Next_Page:
			case Scenario_End:
				break;

			case Scenario_Voice_Play:
			{
				param[L"value"] = m_Reader.Read<int>();
			}
			break;

			default: throw std::runtime_error("Parser::ParseScenarioInstrParam: Error!");
			}

			return param;
		}

		Rut::RxJson::JValue Parse()
		{
			Rut::RxJson::JArray codes;

			while (m_Reader.NextInstr())
			{
				Rut::RxJson::JObject instr;
				instr[L"opcode"] = m_Reader.GetOPCode();
				instr[L"command"] = this->ParseScenarioInstrName();
				instr[L"parameter"] = this->ParseScenarioInstrParam();
				codes.emplace_back(std::move(instr));
			}

			return codes;
		}
	};
}