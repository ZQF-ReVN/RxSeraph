#pragma once
#include <stdexcept>
#include <Rut/RxJson.h>

#include "Script_Reader.h"
#include "Script_Instruction.h"


namespace Seraph::Script::V2
{
	class ExpressionParser
	{
	private:
		Reader& m_Reader;

	public:
		ExpressionParser(Reader& rReader) : m_Reader(rReader)
		{

		}

		std::wstring ParseExpressionInstrName() const
		{
			switch (m_Reader.GetOPCode())
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

		Rut::RxJson::JValue ParseExpressionInstrParam()
		{
			Rut::RxJson::JObject param;

			switch (m_Reader.GetOPCode())
			{
			case Seraph::Script::V2::Expression_Stack0_Get:
			{
				param[L"seq"] = m_Reader.Read<uint8_t>();
			}
			break;

			case Seraph::Script::V2::Expression_Stack1_Check:
			{
				uint8_t type = m_Reader.Read<uint8_t>();
				param[L"flag"] = type;

				if (type == 1)
				{
					param[L"exp"] = this->ParseExpression();
				}
				else
				{
					param[L"seq0"] = m_Reader.Read<uint16_t>();
					param[L"seq1"] = m_Reader.Read<uint8_t>();
				}
			}
			break;

			case Seraph::Script::V2::Expression_Stack2_Get_Type0:
			{
				param[L"seq"] = m_Reader.Read<uint16_t>();
			}
			break;

			case Seraph::Script::V2::Expression_Stack2_Get_Type1:
			{
				param[L"seq"] = m_Reader.Read<uint16_t>();
				param[L"exp"] = this->ParseExpression();
			}
			break;

			case Seraph::Script::V2::Expression_Stack2_Get_Type2:
			{
				param[L"un0"] = m_Reader.Read<uint16_t>();
				param[L"un1"] = m_Reader.Read<uint16_t>();
				param[L"exp0"] = this->ParseExpression();
				param[L"un2"] = m_Reader.Read<uint16_t>();
				param[L"exp1"] = this->ParseExpression();
			}
			break;

			case Seraph::Script::V2::Expression_Dword_Get:
			{
				param[L"val"] = m_Reader.Read<int>();
			}
			break;

			case Seraph::Script::V2::Expression_Res_Get:
			{
				param[L"res_table_seq"] = m_Reader.Read<uint16_t>();
				param[L"size0"] = m_Reader.Read<uint16_t>();
				param[L"exp_size1"] = this->ParseExpression();
				param[L"read_type"] = m_Reader.Read<uint8_t>();
				param[L"res_base"] = m_Reader.Read<uint16_t>();
				param[L"type"] = m_Reader.Read<uint16_t>();
			}
			break;

			case Seraph::Script::V2::Expression_Random_Val_Get:
			{
				param[L"random_val"] = m_Reader.Read<int>();
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
				param[L"pc"] = m_Reader.Read<int>();
			}
			break;

			case Seraph::Script::V2::Expression_End:
				break;

			default: throw std::runtime_error("Parser::ParseExpressionInstrParam: Error!");
			}

			return param;
		}

		Rut::RxJson::JValue ParseExpression()
		{
			Rut::RxJson::JArray codes;

			while (m_Reader.NextInstr())
			{
				Rut::RxJson::JObject instr;
				instr[L"opcode"] = m_Reader.GetOPCode();
				instr[L"command"] = this->ParseExpressionInstrName();
				instr[L"parameter"] = this->ParseExpressionInstrParam();
				codes.emplace_back(std::move(instr));
			}

			return codes;
		}
	};
}