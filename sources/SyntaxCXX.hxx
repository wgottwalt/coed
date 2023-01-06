#pragma once

#include <QRegExp>
#include <QTextCharFormat>

#include "Syntax.hxx"

class SyntaxCXX : public Syntax {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit SyntaxCXX(QTextDocument *parent = nullptr);
    SyntaxCXX(const SyntaxCXX &rhs) = delete;
    SyntaxCXX(SyntaxCXX &&rhs) noexcept = delete;
    ~SyntaxCXX() noexcept override = default;

    //--- public operators ---
    SyntaxCXX &operator=(const SyntaxCXX &rhs) = delete;
    SyntaxCXX &operator=(SyntaxCXX &&rhs) noexcept = delete;

    //--- public methods ---
    [[nodiscard]] Types::Syntax type() const noexcept final;
    [[nodiscard]] QString name() const noexcept final;

    void highlightBlock(const QString &text) final;
    void setStandard(Types::CXX mode = Types::CXX::STD98);

private:
    //--- private properties ---
    QTextCharFormat _keywords_format;
    QTextCharFormat _types_format;
    QTextCharFormat _modifiers_format;
    QTextCharFormat _prepro_format;
    QTextCharFormat _slcomment_format;
    QTextCharFormat _mlcomment_format;
    QRegExp _keywords_regexp;
    QRegExp _types_regexp;
    QRegExp _modifiers_regexp;
    QRegExp _prepro_regexp;
    QRegExp _slcomment_regexp;
    QRegExp _mlcomment_start_regexp;
    QRegExp _mlcomment_stop_regexp;
};
