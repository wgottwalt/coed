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

    QString cxxToString(CXX cxx) noexcept;
    CXX stringToCXX(const QString &str) noexcept;

    QString languageToString(Language lang) noexcept;
    Language stringToLanguage(const QString &str) noexcept;

    QString syntaxToString(Syntax syntax) noexcept;
    Syntax stringToSyntax(const QString &str) noexcept;

    QString themeToString(Theme theme) noexcept;
    Theme stringToTheme(const QString &str) noexcept;
}

Q_DECLARE_METATYPE(Types::CXX)
Q_DECLARE_METATYPE(Types::Language)
Q_DECLARE_METATYPE(Types::Syntax)
Q_DECLARE_METATYPE(Types::TextAction)
Q_DECLARE_METATYPE(Types::Theme)
