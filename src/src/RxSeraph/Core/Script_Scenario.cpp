#include <RxSeraph/Core/Script_Scenario.h>
#include <RxSeraph/Core/Script_Expression.h>
#include <RxSeraph/Core/Script_Instruction.h>
#include <ZxCvt/ZxCvt.h>
#include <stdexcept>


namespace ZQF::RxSeraph::Script::V2
{
    Scenario::Scenario(Reader& rReader) : m_Reader(rReader)
    {

    }

    auto Scenario::ParseInstrName() const -> std::string_view
    {
        switch (m_Reader.GetCurOP())
        {
        case RxSeraph::Script::V2::Scenario_Text_Push: return "Scenario_Text_Push";
        case RxSeraph::Script::V2::Scenario_Text_Format: return "Scenario_Text_Format";
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_R: return "Scenario_Font_Set_Color_R";
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_G: return "Scenario_Font_Set_Color_G";
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_B: return "Scenario_Font_Set_Color_B";
        case RxSeraph::Script::V2::Scenario_Font_Set_Width: return "Scenario_Font_Set_Width";
        case RxSeraph::Script::V2::Scenario_Font_Set_Heigh: return "Scenario_Font_Set_Heigh";
        case RxSeraph::Script::V2::Scenario_Un7: return "Scenario_Un7";
        case RxSeraph::Script::V2::Scenario_Time_Wait: return "Scenario_Time_Wait";
        case RxSeraph::Script::V2::Scenario_Text_Allow_Color_Green: return "Scenario_Text_Allow_Color_Green";
        case RxSeraph::Script::V2::Scenario_Text_Draw_Layer: return "Scenario_Text_Draw_Layer";
        case RxSeraph::Script::V2::Scenario_Text_Allow_Vertical: return "Scenario_Text_Allow_Vertical";
        case RxSeraph::Script::V2::Scenario_Text_Indent_Add: return "Scenario_Text_Indent_Add";
        case RxSeraph::Script::V2::Scenario_Se_Play: return "Scenario_Se_Play";
        case RxSeraph::Script::V2::Scenario_Cursor_Allow_Click: return "Scenario_Cursor_Allow_Click";
        case RxSeraph::Script::V2::Scenario_Text_Indent_At: return "Scenario_Text_Indent_At";
        case RxSeraph::Script::V2::Scenario_Next_Line: return "Scenario_Next_Line";
        case RxSeraph::Script::V2::Scenario_Input_Wait: return "Scenario_Input_Wait";
        case RxSeraph::Script::V2::Scenario_Text_Indent_Flag: return "Scenario_Text_Indent_Flag";
        case RxSeraph::Script::V2::Scenario_Next_Page: return "Scenario_Next_Page";
        case RxSeraph::Script::V2::Scenario_Voice_Play: return "Scenario_Voice_Play";
        case RxSeraph::Script::V2::Scenario_Screen_Shake: return "Scenario_Screen_Shake";
        case RxSeraph::Script::V2::Scenario_End: return "Scenario_End";
        default: throw std::runtime_error("Scenario::ParseInstrName: Error Command!");
        }
    }

    auto Scenario::ParseInstrParam() -> ZxJson::JValue
    {
        ZxJson::JObject_t param;

        switch (m_Reader.GetCurOP())
        {
        case RxSeraph::Script::V2::Scenario_Text_Push: { param["value"] = m_Reader.Read<std::string>(); } break;
        case RxSeraph::Script::V2::Scenario_Text_Format: { throw std::runtime_error("Scenario::ParseInstrParam: Error!"); } break;
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_R:
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_G:
        case RxSeraph::Script::V2::Scenario_Font_Set_Color_B:
        case RxSeraph::Script::V2::Scenario_Font_Set_Width:
        case RxSeraph::Script::V2::Scenario_Font_Set_Heigh:
        case RxSeraph::Script::V2::Scenario_Un7:
        case RxSeraph::Script::V2::Scenario_Time_Wait:
        case RxSeraph::Script::V2::Scenario_Text_Allow_Color_Green:
        case RxSeraph::Script::V2::Scenario_Text_Draw_Layer:
        case RxSeraph::Script::V2::Scenario_Text_Allow_Vertical:
        case RxSeraph::Script::V2::Scenario_Text_Indent_Add:
        case RxSeraph::Script::V2::Scenario_Se_Play:
        case RxSeraph::Script::V2::Scenario_Cursor_Allow_Click:
        case RxSeraph::Script::V2::Scenario_Text_Indent_At:
        case RxSeraph::Script::V2::Scenario_Screen_Shake: { param["value"] = m_Reader.Read<uint8_t>(); } break;
        case RxSeraph::Script::V2::Scenario_Next_Line:
        case RxSeraph::Script::V2::Scenario_Input_Wait:
        case RxSeraph::Script::V2::Scenario_Text_Indent_Flag:
        case RxSeraph::Script::V2::Scenario_Next_Page:
        case RxSeraph::Script::V2::Scenario_End: break;
        case RxSeraph::Script::V2::Scenario_Voice_Play: { param["value"] = (int)m_Reader.Read<uint32_t>(); } break;
        default: throw std::runtime_error("Scenario::ParseInstrParam: Error!");
        }

        return param;
    }

    auto Scenario::Parse() -> ZxJson::JValue
    {
        ZxJson::JArray_t codes;

        do
        {
            ZxJson::JObject_t instr
            {
                { "opcode", m_Reader.ReadOP() },
                { "command", this->ParseInstrName() },
                { "parameter", this->ParseInstrParam() }
            };
            codes.emplace_back(std::move(instr));
        } while (m_Reader.GetCurOP() != 0xFF);

        return codes;
    }

    auto Scenario::Make(ZxMem& rfMem, const ZxJson::JValue& rCodes, const std::size_t nCodePage) -> void
    {
        ZxCvt cvt;
        for (const auto& instr : rCodes.GetArray())
        {
            const auto op = instr.At("opcode").GetNum<std::uint8_t>();
            rfMem << op;

            switch (op)
            {
            case RxSeraph::Script::V2::Scenario_Text_Push:
            {
                const auto text_dbcs = cvt.UTF8ToMBCS(instr.At("parameter").At("value").GetStrView(), nCodePage);
                rfMem << std::span{ text_dbcs.data() , text_dbcs.size() + 1 };
            }
            break;

            case RxSeraph::Script::V2::Scenario_Text_Format: { throw std::runtime_error("Scenario::Make: Error!"); } break;
            case RxSeraph::Script::V2::Scenario_Font_Set_Color_R:
            case RxSeraph::Script::V2::Scenario_Font_Set_Color_G:
            case RxSeraph::Script::V2::Scenario_Font_Set_Color_B:
            case RxSeraph::Script::V2::Scenario_Font_Set_Width:
            case RxSeraph::Script::V2::Scenario_Font_Set_Heigh:
            case RxSeraph::Script::V2::Scenario_Un7:
            case RxSeraph::Script::V2::Scenario_Time_Wait:
            case RxSeraph::Script::V2::Scenario_Text_Allow_Color_Green:
            case RxSeraph::Script::V2::Scenario_Text_Draw_Layer:
            case RxSeraph::Script::V2::Scenario_Text_Allow_Vertical:
            case RxSeraph::Script::V2::Scenario_Text_Indent_Add:
            case RxSeraph::Script::V2::Scenario_Se_Play:
            case RxSeraph::Script::V2::Scenario_Cursor_Allow_Click:
            case RxSeraph::Script::V2::Scenario_Text_Indent_At:
            case RxSeraph::Script::V2::Scenario_Screen_Shake: { rfMem << instr.At("parameter").At("value").GetNum<uint8_t>(); } break;
            case RxSeraph::Script::V2::Scenario_Next_Line:
            case RxSeraph::Script::V2::Scenario_Input_Wait:
            case RxSeraph::Script::V2::Scenario_Text_Indent_Flag:
            case RxSeraph::Script::V2::Scenario_Next_Page:
            case RxSeraph::Script::V2::Scenario_End: break;
            case RxSeraph::Script::V2::Scenario_Voice_Play: { rfMem << instr.At("parameter").At("value").GetNum<uint32_t>(); } break;
            default: throw std::runtime_error("Scenario::Make: Error!");
            }
        }
    }
}
