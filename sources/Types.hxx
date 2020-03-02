#pragma once

#include <QObject>

namespace Types
{
    enum class CXX : quint8 {
        STD98,
        STD11,
        STD14,
        STD17,
        STD20
    };

    enum class Language : quint8 {
        Dutch,
        English,
        French,
        Italian,
        German,
        Spanish
    };

    enum class Syntax : quint8 {
        None,
        CXX
    };

    enum class TextAction : quint8 {
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Delete,
        SelectAll
    };

    enum class Theme : quint8 {
        Fusion,
        QtCurve,
        Windows
    };
}

Q_DECLARE_METATYPE(Types::CXX)
Q_DECLARE_METATYPE(Types::Language)
Q_DECLARE_METATYPE(Types::Syntax)
Q_DECLARE_METATYPE(Types::TextAction)
Q_DECLARE_METATYPE(Types::Theme)

const QString cxxToString(const Types::CXX cxx);
Types::CXX stringToCXX(const QString &str);

const QString languageToString(const Types::Language lang);
Types::Language stringToLanguage(const QString &str);

const QString syntaxToString(const Types::Syntax syntax);
Types::Syntax stringToSyntax(const QString &str);

const QString themeToString(const Types::Theme theme);
Types::Theme stringToTheme(const QString &str);
