#pragma once


namespace Seraph::Script::V2
{
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
}