#pragma once

#include <QSyntaxHighlighter>
#include "Types.hxx"

class QTextDocument;

class Syntax : public QSyntaxHighlighter {
    Q_OBJECT
public:
    //--- public constructors ---
    Syntax(QTextDocument *parent = nullptr);
    Syntax(const Syntax &rhs) = delete;
    Syntax(Syntax &&rhs) = delete;
    virtual ~Syntax();

    //--- public operators ---
    Syntax &operator=(const Syntax &rhs) = delete;
    Syntax &operator=(Syntax &&rhs) = delete;

    //--- public methods ---
    static const QString toString(const Types::Syntax syntax);
    virtual Types::Syntax type() const = 0;
    virtual const QString name() const = 0;
};
