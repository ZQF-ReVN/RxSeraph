#pragma once
#include <string_view>
#include <ZxJson/JValue.h>
#include <RxSeraph/Script_Reader.h>


namespace ZQF::RxSeraph::Script::V2
{
	class Expression
	{
	private:
		Reader& m_Reader;

	public:
        Expression(Reader& rReader);

    private:
        auto ParseInstrName() const -> std::string_view;
        auto ParseInstrParam() -> ZxJson::JValue;

    public:
        auto ParseExpression() -> ZxJson::JValue;

	};
}
