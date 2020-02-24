#include "Types.hxx"

const QString cxxToString(const Types::CXX cxx)
{
    switch (cxx)
    {
        case Types::CXX::STD11:
            return "STD11";

        case Types::CXX::STD14:
            return "STD14";

        case Types::CXX::STD17:
            return "STD17";

        case Types::CXX::STD20:
            return "STD20";

        case Types::CXX::STD98:
        default:
            return "STD98";
    }
}

Types::CXX stringToCXX(const QString &str)
{
    if (str == "STD11")
        return Types::CXX::STD11;

    if (str == "STD14")
        return Types::CXX::STD14;

    if (str == "STD17")
        return Types::CXX::STD17;

    if (str == "STD20")
        return Types::CXX::STD20;

    return Types::CXX::STD98;
}

const QString languageToString(const Types::Language lang)
{
    switch (lang)
    {
        case Types::Language::Dutch:
            return "Dutch";

        case Types::Language::French:
            return "French";

        case Types::Language::Italian:
            return "Italian";

        case Types::Language::German:
            return "German";

        case Types::Language::Spanish:
            return "Spanish";

        case Types::Language::English:
        default:
            return "English";
    }
}

Types::Language stringToLanguage(const QString &str)
{
    if (str == "Dutch")
        return Types::Language::Dutch;

    if (str == "French")
        return Types::Language::French;

    if (str == "Italian")
        return Types::Language::Italian;

    if (str == "German")
        return Types::Language::German;

    if (str == "Spanish")
        return Types::Language::Spanish;

    return Types::Language::English;
}

const QString syntaxToString(const Types::Syntax syntax)
{
    switch (syntax)
    {
        case Types::Syntax::CXX:
            return "CXX";

        case Types::Syntax::None:
        default:
            return "None";
    }
}

Types::Syntax stringToSyntax(const QString &str)
{
    if (str == "CXX")
        return Types::Syntax::CXX;

    return Types::Syntax::None;
}

const QString themeToString(const Types::Theme theme)
{
    switch (theme)
    {
        case Types::Theme::QtCurve:
            return "QtCurve";

        case Types::Theme::Windows:
            return "Windows";

        case Types::Theme::Fusion:
        default:
            return "Fusion";
    }
}

Types::Theme stringToTheme(const QString &str)
{
    if (str == "QtCurve")
        return Types::Theme::QtCurve;

    if (str == "Windows")
        return Types::Theme::Windows;

    return Types::Theme::Fusion;
}
