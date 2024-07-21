#pragma once
#include <span>
#include <string>
#include <vector>
#include <cstdint>
#include <ZxCvt/ZxCvt.h>
#include <ZxJson/JValue.h>
#include <RxSeraph/Script_Reader.h>
#include <RxSeraph/Script_Scenario.h>
#include <RxSeraph/Script_Expression.h>


namespace ZQF::RxSeraph::Script::V2
{
    class Parser
    {
    private:
        ZxCvt m_Cvt;
        Reader m_Reader;
        Scenario m_Scenario;
        Expression m_Expression;

    public:
        Parser(std::span<std::uint8_t> spCode, const std::size_t nCodePage);

    private:
        auto ParseCharName(ZxJson::JArray_t& jaCodes, std::vector<std::string>& vcNameTable, const std::size_t nCharNameCodeSize) -> void;
        auto ParseScenario(ZxJson::JArray_t& jaCodes) -> void;
        auto ParseSelect(ZxJson::JArray_t& jaCodes, const std::size_t nSelectCodeSize) -> void;

    public:
        auto Parse(std::vector<std::string>& vcNameTable) -> ZxJson::JValue;

    };
}
