#include <iostream>
#include <memory>
#include <variant>
#include <Rut/RxJson.h>
#include <Rut/RxStr.h>
#include <Seraph/Dat.h>

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
public:
	uint8_t m_ucOP = 0;
	uint32_t m_nPC = 0;
	const uint8_t* m_pCode = nullptr;

	ScenarioParser(const uint8_t* const pCode) 
	{
		m_pCode = pCode;
	}

	template<class T> auto Read()
	{
		if constexpr (std::is_same_v<T, std::wstring>)
		{
			char* str_ptr = (char*)(m_pCode + m_nPC);
			size_t str_len = strlen(str_ptr);
			m_nPC += str_len + 1;
			return Rut::RxStr::ToWCS({ str_ptr ,str_len }, 932);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			char* str_ptr = (char*)(m_pCode + m_nPC);
			size_t str_len = strlen(str_ptr);
			m_nPC += str_len + 1;
			return { str_ptr ,str_len };
		}
		else
		{
			T val = *(T*)(m_pCode + m_nPC);
			m_nPC += sizeof(T);
			return val;
		}
	}

	bool NextInstr()
	{
		m_ucOP = this->Read<uint8_t>();
		return m_ucOP != Scenario_End ? true : false;
	}

	uint8_t GetOPCode() const
	{
		return m_ucOP;
	}

	std::wstring ParseInstrName() const
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

	Rut::RxJson::JObject ParseInstrParam()
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

	void Parse()
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

		Rut::RxJson::Parser::Save(codes, L"codes.json", true);
	}

};




int main()
{
	uint8_t test_code[] = {
			0x08, 0x0B, 0x09, 0x00, 0x08, 0x0B, 0x00, 0x81, 0x5C, 0x81, 0x5C, 0x81, 0x5C, 0x81, 0x77, 0x8E,
			0x84, 0x97, 0xA7, 0x92, 0xE9, 0x96, 0x50, 0x81, 0x69, 0x82, 0xC4, 0x82, 0xA2, 0x82, 0xD9, 0x82,
			0xA4, 0x81, 0x6A, 0x8A, 0x77, 0x89, 0x80, 0x81, 0x78, 0x00, 0x08, 0x32, 0x00, 0x81, 0x42, 0x00,
			0x08, 0x0B, 0x08, 0x0B, 0x15, 0x17, 0x08, 0x0B, 0x00, 0x97, 0x63, 0x93, 0x99, 0x95, 0x94, 0x82,
			0xA9, 0x82, 0xE7, 0x91, 0xE5, 0x8A, 0x77, 0x95, 0x94, 0x82, 0xDC, 0x82, 0xC5, 0x94, 0xF5, 0x82,
			0xA6, 0x00, 0x08, 0x1E, 0x00, 0x81, 0x41, 0x00, 0x08, 0x0B, 0x00, 0x8B, 0xDF, 0x97, 0xD7, 0x88,
			0xC8, 0x8A, 0x4F, 0x82, 0xC9, 0x82, 0xE0, 0x96, 0xBC, 0x82, 0xCC, 0x92, 0x6D, 0x82, 0xE7, 0x82,
			0xEA, 0x82, 0xBD, 0x00, 0x14, 0x00, 0x83, 0x47, 0x83, 0x58, 0x83, 0x4A, 0x83, 0x8C, 0x81, 0x5B,
			0x83, 0x5E, 0x81, 0x5B, 0x8E, 0xAE, 0x82, 0xCC, 0x96, 0xBC, 0x96, 0xE5, 0x8A, 0x77, 0x89, 0x80,
			0x00, 0x08, 0x32, 0x00, 0x81, 0x42, 0x00, 0x08, 0x0B, 0x08, 0x0B, 0x15, 0x17, 0x08, 0x0B, 0x00,
			0x82, 0xBB, 0x82, 0xB1, 0x82, 0xC9, 0x82, 0xCD, 0x96, 0xBC, 0x96, 0xE5, 0x82, 0xC9, 0x82, 0xD3,
			0x82, 0xB3, 0x82, 0xED, 0x82, 0xB5, 0x82, 0xA2, 0x89, 0xC6, 0x95, 0xBF, 0x82, 0xE2, 0x8E, 0x91,
			0x8E, 0x59, 0x82, 0xF0, 0x8E, 0x9D, 0x82, 0xC1, 0x82, 0xBD, 0x90, 0x6C, 0x8A, 0xD4, 0x81, 0x5C,
			0x81, 0x5C, 0x81, 0x5C, 0x00, 0x14, 0x00, 0x82, 0xA2, 0x82, 0xED, 0x82, 0xE4, 0x82, 0xE9, 0x82,
			0xA8, 0x96, 0x56, 0x82, 0xBF, 0x82, 0xE1, 0x00, 0x08, 0x0F, 0x00, 0x82, 0xDC, 0x81, 0x45, 0x00,
			0x08, 0x0B, 0x00, 0x82, 0xA8, 0x8F, 0xEC, 0x97, 0x6C, 0x82, 0xAA, 0x8F, 0x57, 0x82, 0xDC, 0x82,
			0xC1, 0x82, 0xC4, 0x82, 0xA2, 0x82, 0xE9, 0x00, 0x08, 0x32, 0x00, 0x81, 0x42, 0x00, 0x08, 0x0B,
			0x08, 0x0B, 0x15, 0x17, 0x08, 0x0B, 0x00, 0x82, 0xBB, 0x82, 0xCC, 0x8A, 0x77, 0x89, 0x80, 0x82,
			0xCC, 0x88, 0xEA, 0x8E, 0xBA, 0x81, 0x5C, 0x81, 0x5C, 0x81, 0x5C, 0x91, 0xBD, 0x90, 0x94, 0x82,
			0xCC, 0x90, 0xB6, 0x93, 0x6B, 0x82, 0xAA, 0x8F, 0x57, 0x82, 0xDC, 0x82, 0xE9, 0x95, 0x94, 0x89,
			0xAE, 0x82, 0xCC, 0x92, 0x86, 0x82, 0xC5, 0x00, 0x08, 0x1E, 0x00, 0x81, 0x41, 0x00, 0x08, 0x0B,
			0x08, 0x0B, 0x14, 0x00, 0x88, 0xEA, 0x90, 0x6C, 0x82, 0xCC, 0x92, 0x6A, 0x8E, 0x71, 0x90, 0xB6,
			0x93, 0x6B, 0x82, 0xAA, 0x8B, 0x63, 0x91, 0xE8, 0x82, 0xF0, 0x93, 0xC7, 0x82, 0xDD, 0x8F, 0xE3,
			0x82, 0xB0, 0x82, 0xC4, 0x82, 0xA2, 0x82, 0xBD, 0x00, 0x08, 0x32, 0x00, 0x81, 0x42, 0x00, 0x08,
			0x0B, 0x08, 0x0B, 0x15, 0x08, 0x0B, 0xFF, 0x2C, 0x00, 0x02, 0x0B, 0x00, 0xFF
	};

	ScenarioParser parse(test_code);
	parse.Parse();


	//Seraph::Dat data1;
	//data1.Unpack(L"ArchPac.Dat",0x05EBB40D);

	//Seraph::Dat data2;
	//data2.UnpackScnPac(L"ScnPac.Dat");
	int a = 0;
}