#pragma once

#include <QRegExp>
#include <QTextCharFormat>
#include "Syntax.hxx"

class SyntaxCXX : public Syntax {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit SyntaxCXX(QTextDocument *parent = nullptr) noexcept(false);
    SyntaxCXX(const SyntaxCXX &rhs) noexcept(false) = delete;
    SyntaxCXX(SyntaxCXX &&rhs) noexcept = delete;
    virtual ~SyntaxCXX() noexcept = default;

    //--- public operators ---
    SyntaxCXX &operator=(const SyntaxCXX &rhs) noexcept(false) = delete;
    SyntaxCXX &operator=(SyntaxCXX &&rhs) noexcept = delete;

    //--- public methods ---
    virtual Types::Syntax type() const noexcept override final;
    virtual const QString name() const noexcept override final;

    virtual void highlightBlock(const QString &text) noexcept(false) override final;
    void setStandard(const Types::CXX mode = Types::CXX::STD98) noexcept(false);

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
