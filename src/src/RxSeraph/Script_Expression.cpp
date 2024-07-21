#include <RxSeraph/Script_Expression.h>
#include <RxSeraph/Script_Instruction.h>
#include <cstdint>
#include <stdexcept>


namespace ZQF::RxSeraph::Script::V2
{
    Expression::Expression(Reader& rReader) : m_Reader(rReader)
    {

    }

    auto Expression::ParseInstrName() const -> std::string_view
    {
        switch (m_Reader.GetCurOP())
        {
        case RxSeraph::Script::V2::Expression_Stack0_Get: return "Expression_Stack0_Get";
        case RxSeraph::Script::V2::Expression_Stack1_Check: return "Expression_Stack1_Check";
        case RxSeraph::Script::V2::Expression_Stack2_Get_Type0: return "Expression_Stack2_Get_Type0";
        case RxSeraph::Script::V2::Expression_Stack2_Get_Type1: return "Expression_Stack2_Get_Type1";
        case RxSeraph::Script::V2::Expression_Stack2_Get_Type2: return "Expression_Stack2_Get_Type2";
        case RxSeraph::Script::V2::Expression_Dword_Get: return "Expression_Dword_Get";
        case RxSeraph::Script::V2::Expression_Res_Get: return "Expression_Res_Get";
        case RxSeraph::Script::V2::Expression_Random_Val_Get: return "Expression_Random_Val_Get";
        case RxSeraph::Script::V2::Expression_Addition: return "Expression_Addition";
        case RxSeraph::Script::V2::Expression_Subtraction: return "Expression_Subtraction";
        case RxSeraph::Script::V2::Expression_Multiplication: return "Expression_Multiplication";
        case RxSeraph::Script::V2::Expression_Division: return "Expression_Division";
        case RxSeraph::Script::V2::Expression_Modulus: return "Expression_Modulus";
        case RxSeraph::Script::V2::Expression_AND: return "Expression_AND";
        case RxSeraph::Script::V2::Expression_OR: return "Expression_OR";
        case RxSeraph::Script::V2::Expression_XOR: return "Expression_XOR";
        case RxSeraph::Script::V2::Expression_IF: return "Expression_IF";
        case RxSeraph::Script::V2::Expression_Equal: return "Expression_Equal";
        case RxSeraph::Script::V2::Expression_NotEqual: return "Expression_NotEqual";
        case RxSeraph::Script::V2::Expression_LessThanEqual: return "Expression_LessThanEqual";
        case RxSeraph::Script::V2::Expression_GreaterThanEqual: return "Expression_GreaterThanEqual";
        case RxSeraph::Script::V2::Expression_LessThan: return "Expression_LessThan";
        case RxSeraph::Script::V2::Expression_GreaterThan: return "Expression_GreaterThan";
        case RxSeraph::Script::V2::Expression_LogicalAND: return "Expression_LogicalAND";
        case RxSeraph::Script::V2::Expression_LogicalOR: return "Expression_LogicalOR";
        case RxSeraph::Script::V2::Expression_PC_Set: return "Expression_PC_Set";
        case RxSeraph::Script::V2::Expression_End: return "Expression_End";
        default: throw std::runtime_error("Expression::ParseName: Error!");
        };
    }

    auto Expression::ParseInstrParam() -> ZxJson::JValue
    {
        ZxJson::JObject_t param;

        switch (m_Reader.GetCurOP())
        {
        case RxSeraph::Script::V2::Expression_Stack0_Get:
        {
            param["seq"] = m_Reader.Read<uint8_t>();
        }
        break;

        case RxSeraph::Script::V2::Expression_Stack1_Check:
        {
            uint8_t type = m_Reader.Read<uint8_t>();
            param["flag"] = type;

            if (type == 1)
            {
                param["exp"] = this->ParseExpression();
            }
            else
            {
                param["seq0"] = m_Reader.Read<uint16_t>();
                param["seq1"] = m_Reader.Read<uint8_t>();
            }
        }
        break;

        case RxSeraph::Script::V2::Expression_Stack2_Get_Type0:
        {
            param["seq"] = m_Reader.Read<uint16_t>();
        }
        break;

        case RxSeraph::Script::V2::Expression_Stack2_Get_Type1:
        {
            param["seq"] = m_Reader.Read<uint16_t>();
            param["exp"] = this->ParseExpression();
        }
        break;

        case RxSeraph::Script::V2::Expression_Stack2_Get_Type2:
        {
            param["un0"] = m_Reader.Read<uint16_t>();
            param["un1"] = m_Reader.Read<uint16_t>();
            param["exp0"] = this->ParseExpression();
            param["un2"] = m_Reader.Read<uint16_t>();
            param["exp1"] = this->ParseExpression();
        }
        break;

        case RxSeraph::Script::V2::Expression_Dword_Get:
        {
            param["val"] = m_Reader.Read<int>();
        }
        break;

        case RxSeraph::Script::V2::Expression_Res_Get:
        {
            param["res_table_seq"] = m_Reader.Read<uint16_t>();
            param["size0"] = m_Reader.Read<uint16_t>();
            param["exp_size1"] = this->ParseExpression();
            param["read_type"] = m_Reader.Read<uint8_t>();
            param["res_base"] = m_Reader.Read<uint16_t>();
            param["type"] = m_Reader.Read<uint16_t>();
        }
        break;

        case RxSeraph::Script::V2::Expression_Random_Val_Get:
        {
            param["random_val"] = m_Reader.Read<int>();
        }
        break;

        case RxSeraph::Script::V2::Expression_Addition:
        case RxSeraph::Script::V2::Expression_Subtraction:
        case RxSeraph::Script::V2::Expression_Multiplication:
        case RxSeraph::Script::V2::Expression_Division:
        case RxSeraph::Script::V2::Expression_Modulus:
        case RxSeraph::Script::V2::Expression_AND:
        case RxSeraph::Script::V2::Expression_OR:
        case RxSeraph::Script::V2::Expression_XOR: break;

        case RxSeraph::Script::V2::Expression_IF:
        {
            // ??
            throw std::runtime_error("Expression::ParseInstrParam: Error!");
        }
        break;

        case RxSeraph::Script::V2::Expression_Equal:
        case RxSeraph::Script::V2::Expression_NotEqual:
        case RxSeraph::Script::V2::Expression_LessThanEqual:
        case RxSeraph::Script::V2::Expression_GreaterThanEqual:
        case RxSeraph::Script::V2::Expression_LessThan:
        case RxSeraph::Script::V2::Expression_GreaterThan:
        case RxSeraph::Script::V2::Expression_LogicalAND:
        case RxSeraph::Script::V2::Expression_LogicalOR:
        case RxSeraph::Script::V2::Expression_End: break;

        case RxSeraph::Script::V2::Expression_PC_Set:
        {
            param["pc"] = m_Reader.Read<int>();
        }
        break;

        default: throw std::runtime_error("Expression::ParseInstrParam: Error!");
        }

        return param;
    }

    auto Expression::ParseExpression() -> ZxJson::JValue
    {
        ZxJson::JArray_t codes;

        do
        {
            ZxJson::JObject_t instr;
            instr["opcode"] = m_Reader.ReadOP();
            instr["command"] = this->ParseInstrName();
            instr["parameter"] = this->ParseInstrParam();
            codes.emplace_back(std::move(instr));
        } while (m_Reader.GetCurOP() != 0xFF);

        return codes;
    }
}
