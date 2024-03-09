#pragma once
#include <span>
#include <memory>
#include <ranges>

#include <Rut/RxMem.h>
#include <Rut/RxStr.h>
#include <Rut/RxJson.h>


namespace Seraph::Script::V2
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

	enum ExpressionInstruction
	{
		Expression_Stack0_Get,
		Expression_Stack1_Check,
		Expression_Stack2_Get_Type0,
		Expression_Stack2_Get_Type1,
		Expression_Stack2_Get_Type2,
		Expression_Dword_Get,
		Expression_Res_Get,
		Expression_Random_Val_Get,
		Expression_Addition,
		Expression_Subtraction,
		Expression_Multiplication,
		Expression_Division,
		Expression_Modulus,
		Expression_AND,
		Expression_OR,
		Expression_XOR,
		Expression_IF,
		Expression_Equal,
		Expression_NotEqual,
		Expression_LessThanEqual,
		Expression_GreaterThanEqual,
		Expression_LessThan,
		Expression_GreaterThan,
		Expression_LogicalAND,
		Expression_LogicalOR,
		Expression_PC_Set,
		Expression_End = 0xFF
	};

	// class SystemCommand::DrawSelect::Parser

	class Parser
	{
	private:
		uint8_t m_ucOP = 0;
		uint32_t m_nPC = 0;
		const std::span<uint8_t> m_spCode;

	public:
		Parser(std::span<uint8_t> spCode);

	private:
		template<class T> auto Read();

	private:
		void SetPC(uint32_t uiPC);
		uint32_t GetPC() const;
		void SkipPC(uint32_t uiPC);
		bool NextInstr();
		uint8_t GetOPCode() const;

	private:
		Rut::RxJson::JValue ParseScenario();
		Rut::RxJson::JValue ParseExpression();
		std::wstring ParseScenarioInstrName() const;
		std::wstring ParseExpressionInstrName() const;
		Rut::RxJson::JValue ParseScenarioInstrParam();
		Rut::RxJson::JValue ParseExpressionInstrParam();

	public:
		Rut::RxJson::JValue Parse(std::unordered_map<size_t, std::wstring>& rNameTable);

	};

	Parser::Parser(std::span<uint8_t> spCode) : m_spCode(spCode)
	{

	}

	template<class T> auto Parser::Read()
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

	void Parser::SetPC(uint32_t uiPC)
	{
		m_nPC = uiPC;
	}

	uint32_t Parser::GetPC() const
	{
		return m_nPC;
	}

	void Parser::SkipPC(uint32_t uiPC)
	{
		m_nPC += uiPC;
	}

	bool Parser::NextInstr()
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

	uint8_t Parser::GetOPCode() const
	{
		return m_ucOP;
	}

	Rut::RxJson::JValue Parser::ParseScenario()
	{
		Rut::RxJson::JArray codes;

		while (this->NextInstr())
		{
			Rut::RxJson::JObject instr;
			instr[L"opcode"] = this->GetOPCode();
			instr[L"command"] = this->ParseScenarioInstrName();
			instr[L"parameter"] = this->ParseScenarioInstrParam();
			codes.emplace_back(std::move(instr));
		}

		return codes;
	}

	Rut::RxJson::JValue Parser::ParseExpression()
	{
		Rut::RxJson::JArray codes;

		while (this->NextInstr())
		{
			Rut::RxJson::JObject instr;
			instr[L"opcode"] = this->GetOPCode();
			instr[L"command"] = this->ParseExpressionInstrName();
			instr[L"parameter"] = this->ParseExpressionInstrParam();
			codes.emplace_back(std::move(instr));
		}

		return codes;
	}
	
	std::wstring Parser::ParseScenarioInstrName() const
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
		default: throw std::runtime_error("Parser::ParseScenarioInstrName: Error Command!");
		}
	}

	std::wstring Parser::ParseExpressionInstrName() const
	{
		switch (this->GetOPCode())
		{
		case Seraph::Script::V2::Expression_Stack0_Get: return L"Expression_Stack0_Get";
		case Seraph::Script::V2::Expression_Stack1_Check: return L"Expression_Stack1_Check";
		case Seraph::Script::V2::Expression_Stack2_Get_Type0: return L"Expression_Stack2_Get_Type0";
		case Seraph::Script::V2::Expression_Stack2_Get_Type1: return L"Expression_Stack2_Get_Type1";
		case Seraph::Script::V2::Expression_Stack2_Get_Type2: return L"Expression_Stack2_Get_Type2";
		case Seraph::Script::V2::Expression_Dword_Get: return L"Expression_Dword_Get";
		case Seraph::Script::V2::Expression_Res_Get: return L"Expression_Res_Get";
		case Seraph::Script::V2::Expression_Random_Val_Get: return L"Expression_Random_Val_Get";
		case Seraph::Script::V2::Expression_Addition: return L"Expression_Addition";
		case Seraph::Script::V2::Expression_Subtraction: return L"Expression_Subtraction";
		case Seraph::Script::V2::Expression_Multiplication: return L"Expression_Multiplication";
		case Seraph::Script::V2::Expression_Division: return L"Expression_Division";
		case Seraph::Script::V2::Expression_Modulus: return L"Expression_Modulus";
		case Seraph::Script::V2::Expression_AND: return L"Expression_AND";
		case Seraph::Script::V2::Expression_OR: return L"Expression_OR";
		case Seraph::Script::V2::Expression_XOR: return L"Expression_XOR";
		case Seraph::Script::V2::Expression_IF: return L"Expression_IF";
		case Seraph::Script::V2::Expression_Equal: return L"Expression_Equal";
		case Seraph::Script::V2::Expression_NotEqual: return L"Expression_NotEqual";
		case Seraph::Script::V2::Expression_LessThanEqual: return L"Expression_LessThanEqual";
		case Seraph::Script::V2::Expression_GreaterThanEqual: return L"Expression_GreaterThanEqual";
		case Seraph::Script::V2::Expression_LessThan: return L"Expression_LessThan";
		case Seraph::Script::V2::Expression_GreaterThan: return L"Expression_GreaterThan";
		case Seraph::Script::V2::Expression_LogicalAND: return L"Expression_LogicalAND";
		case Seraph::Script::V2::Expression_LogicalOR: return L"Expression_LogicalOR";
		case Seraph::Script::V2::Expression_PC_Set: return L"Expression_PC_Set";
		case Seraph::Script::V2::Expression_End: return L"Expression_End";
		default: throw std::runtime_error("Parser::ParseExpressionInstrName: Error!");
		};
	}

	Rut::RxJson::JValue Parser::ParseScenarioInstrParam()
	{
		Rut::RxJson::JObject param;

		switch (this->GetOPCode())
		{
		case Scenario_Text_Push:
		{
			param[L"value"] = this->Read<std::wstring>();
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
			param[L"value"] = this->Read<uint8_t>();
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
			param[L"value"] = this->Read<int>();
		}
		break;

		default: throw std::runtime_error("Parser::ParseScenarioInstrParam: Error!");
		}

		return param;
	}

	Rut::RxJson::JValue Parser::ParseExpressionInstrParam()
	{
		Rut::RxJson::JObject param;

		switch (this->GetOPCode())
		{
		case Seraph::Script::V2::Expression_Stack0_Get:
		{
			param[L"seq"] = this->Read<uint8_t>();
		}
		break;

		case Seraph::Script::V2::Expression_Stack1_Check:
		{
			uint8_t type = this->Read<uint8_t>();
			param[L"flag"] = type;

			if (type == 1)
			{
				param[L"exp"] = this->ParseExpression();
			}
			else
			{
				param[L"seq0"] = this->Read<uint16_t>();
				param[L"seq1"] = this->Read<uint8_t>();
			}
		}
		break;

		case Seraph::Script::V2::Expression_Stack2_Get_Type0:
		{
			param[L"seq"] = this->Read<uint16_t>();
		}
		break;

		case Seraph::Script::V2::Expression_Stack2_Get_Type1:
		{
			param[L"seq"] = this->Read<uint16_t>();
			param[L"exp"] = this->ParseExpression();
		}
		break;

		case Seraph::Script::V2::Expression_Stack2_Get_Type2:
		{
			param[L"un0"] = this->Read<uint16_t>();
			param[L"un1"] = this->Read<uint16_t>();
			param[L"exp0"] = this->ParseExpression();
			param[L"un2"] = this->Read<uint16_t>();
			param[L"exp1"] = this->ParseExpression();
		}
		break;

		case Seraph::Script::V2::Expression_Dword_Get:
		{
			param[L"val"] = this->Read<int>();
		}
		break;

		case Seraph::Script::V2::Expression_Res_Get:
		{
			param[L"res_table_seq"] = this->Read<uint16_t>();
			param[L"size0"] = this->Read<uint16_t>();
			param[L"exp_size1"] = this->ParseExpression();
			param[L"read_type"] = this->Read<uint8_t>();
			param[L"res_base"] = this->Read<uint16_t>();
			param[L"type"] = this->Read<uint16_t>();
		}
		break;

		case Seraph::Script::V2::Expression_Random_Val_Get:
		{
			param[L"random_val"] = this->Read<int>();
		}
		break;

		case Seraph::Script::V2::Expression_Addition:
		case Seraph::Script::V2::Expression_Subtraction:
		case Seraph::Script::V2::Expression_Multiplication:
		case Seraph::Script::V2::Expression_Division:
		case Seraph::Script::V2::Expression_Modulus:
		case Seraph::Script::V2::Expression_AND:
		case Seraph::Script::V2::Expression_OR:
		case Seraph::Script::V2::Expression_XOR:
			break;

		case Seraph::Script::V2::Expression_IF:
		{
			// ??
			throw std::runtime_error("Parser::ParseExpressionInstrParam: Error!");
		}
		break;

		case Seraph::Script::V2::Expression_Equal:
		case Seraph::Script::V2::Expression_NotEqual:
		case Seraph::Script::V2::Expression_LessThanEqual:
		case Seraph::Script::V2::Expression_GreaterThanEqual:
		case Seraph::Script::V2::Expression_LessThan:
		case Seraph::Script::V2::Expression_GreaterThan:
		case Seraph::Script::V2::Expression_LogicalAND:
		case Seraph::Script::V2::Expression_LogicalOR:
			break;

		case Seraph::Script::V2::Expression_PC_Set:
		{
			param[L"pc"] = this->Read<int>();
		}
		break;

		case Seraph::Script::V2::Expression_End:
			break;

		default: throw std::runtime_error("Parser::ParseExpressionInstrParam: Error!");
		}

		return param;
	}

	Rut::RxJson::JValue Parser::Parse(std::unordered_map<size_t, std::wstring>& rNameTable)
	{
		uint8_t search_draw_select_box[] = 
		{
			0x56, 0x05, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00,
			0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF
		};

		uint8_t search_call_draw_text_box_into_scen[] = 
		{ 
			0xFF, 0x0F, 0x09, 0x1F, 0x00, 0x00, 0x47
		};

		Rut::RxJson::JArray codes;
		for (; ;)
		{
			if (this->GetPC() >= m_spCode.size()) { break; }

			uint8_t* cur_ptr = m_spCode.data() + this->GetPC();

			if (memcmp(cur_ptr, search_call_draw_text_box_into_scen, sizeof(search_call_draw_text_box_into_scen)) == 0)
			{
				// get character name
				uint8_t* param_ptr = m_spCode.data() + this->GetPC() - 7;
				if (param_ptr[1] == 0x00 && param_ptr[2] == 0x05 && param_ptr[7] == 0xFF)
				{
					uint32_t char_name_inmage_seq = *(uint32_t*)(param_ptr + 3);
					Rut::RxJson::JObject char_name_json;
					char_name_json[L"self"] = L"character";
					char_name_json[L"name"] = rNameTable[char_name_inmage_seq];
					codes.emplace_back(std::move(char_name_json));
				}
				else
				{
					throw std::runtime_error("Parser::Parse: not find character name");
				}

				// parse scenario codes
				this->SkipPC(sizeof(search_call_draw_text_box_into_scen));
				Rut::RxJson::JObject msg_json;
				msg_json[L"self"] = L"msg";
				msg_json[L"begin"] = (int)this->GetPC();
				msg_json[L"texts"] = this->ParseScenario();
				codes.emplace_back(std::move(msg_json));
			}
			else if (memcmp(cur_ptr, search_draw_select_box, sizeof(search_draw_select_box)) == 0) // parse DrawSelect
			{
				// get select texts
				uint8_t* text_0_ptr = cur_ptr + sizeof(search_draw_select_box) + 0xA;
				std::string_view text_0 = (char*)text_0_ptr;
				uint8_t* text_1_ptr = text_0_ptr + text_0.size() + 1;
				std::string_view text_1 = (char*)text_1_ptr;

				// gen select json
				Rut::RxJson::JObject select_json;
				select_json[L"self"] = L"select";
				select_json[L"begin"] = (int)this->GetPC();
				Rut::RxJson::JArray& select_text_jarray = select_json[L"texts"].ToAry();
				select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_0, 932));
				select_text_jarray.emplace_back(Rut::RxStr::ToWCS(text_1, 932));

				// add json
				codes.emplace_back(std::move(select_json));

				// skip draw select box code
				this->SkipPC(sizeof(search_draw_select_box) + 0xA + text_0.size() + text_1.size() + 2 + 1);
			}
			else
			{
				this->SkipPC(1);
			}
		}

		return codes;
	}

}
