#include "Types.hxx"

namespace Types
{
    const QString cxxToString(const CXX cxx) noexcept
    {
        switch (cxx)
        {
            case CXX::STD11:
                return "STD11";

            case CXX::STD14:
                return "STD14";

            case CXX::STD17:
                return "STD17";

            case CXX::STD20:
                return "STD20";

            case CXX::STD98:
            default:
                return "STD98";
        }
    }

    CXX stringToCXX(const QString &str) noexcept
    {
        if (str == "STD11")
            return CXX::STD11;

        if (str == "STD14")
            return CXX::STD14;

        if (str == "STD17")
            return CXX::STD17;

        if (str == "STD20")
            return CXX::STD20;

        return CXX::STD98;
    }

    const QString languageToString(const Language lang) noexcept
    {
        switch (lang)
        {
            case Language::Dutch:
                return "Dutch";

            case Language::French:
                return "French";

            case Language::Italian:
                return "Italian";

            case Language::German:
                return "German";

            case Language::Spanish:
                return "Spanish";

            case Language::English:
            default:
                return "English";
        }
    }

    Language stringToLanguage(const QString &str) noexcept
    {
        if (str == "Dutch")
            return Language::Dutch;

        if (str == "French")
            return Language::French;

        if (str == "Italian")
            return Language::Italian;

        if (str == "German")
            return Language::German;

        if (str == "Spanish")
            return Language::Spanish;

        return Language::English;
    }

    const QString syntaxToString(const Syntax syntax) noexcept
    {
        switch (syntax)
        {
            case Syntax::CXX:
                return "CXX";

            case Syntax::None:
            default:
                return "None";
        }
    }

    Syntax stringToSyntax(const QString &str) noexcept
    {
        if (str == "CXX")
            return Syntax::CXX;

        return Syntax::None;
    }

    const QString themeToString(const Theme theme) noexcept
    {
        switch (theme)
        {
            case Theme::QtCurve:
                return "QtCurve";

            case Theme::Windows:
                return "Windows";

            case Theme::Fusion:
            default:
                return "Fusion";
        }
    }

    Theme stringToTheme(const QString &str) noexcept
    {
        if (str == "QtCurve")
            return Theme::QtCurve;

        if (str == "Windows")
            return Theme::Windows;

        return Theme::Fusion;
    }
}
