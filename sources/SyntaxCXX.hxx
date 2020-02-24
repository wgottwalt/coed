#pragma once

#include <QRegExp>
#include <QTextCharFormat>
#include "Syntax.hxx"

class SyntaxCXX : public Syntax {
    Q_OBJECT
public:
    //--- public constructors ---
    SyntaxCXX(QTextDocument *parent = nullptr);
    SyntaxCXX(const SyntaxCXX &rhs) = delete;
    SyntaxCXX(SyntaxCXX &&rhs) = delete;
    virtual ~SyntaxCXX();

    //--- public operators ---
    SyntaxCXX &operator=(const SyntaxCXX &rhs) = delete;
    SyntaxCXX &operator=(SyntaxCXX &&rhs) = delete;

    //--- public methods ---
    virtual Types::Syntax type() const override final;
    virtual const QString name() const override final;

    virtual void highlightBlock(const QString &text) override final;
    void setStandard(const Types::CXX mode = Types::CXX::STD98);

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
