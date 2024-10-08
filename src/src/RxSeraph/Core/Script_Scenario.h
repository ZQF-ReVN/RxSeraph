#pragma once
#include <cstdint>
#include <string_view>
#include <ZxMem/ZxMem.h>
#include <ZxJson/JValue.h>
#include <RxSeraph/Core/Script_Reader.h>


namespace ZQF::RxSeraph::Script::V2
{
    class Scenario
    {
    private:
        Reader& m_Reader;

    public:
        Scenario(Reader& rReader);

    private:
        auto ParseInstrName() const -> std::string_view;
        auto ParseInstrParam() -> ZxJson::JValue;

    public:
        auto Parse() -> ZxJson::JValue;
        static auto Make(ZxMem& rfMem, const ZxJson::JValue& rCodes, const std::size_t nCodePage) -> void;
    };
}
