#pragma once

#include <QSyntaxHighlighter>
#include "Types.hxx"

class QTextDocument;

class Syntax : public QSyntaxHighlighter {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit Syntax(QTextDocument *parent = nullptr) noexcept(false);
    Syntax(const Syntax &rhs) noexcept(false) = delete;
    Syntax(Syntax &&rhs) noexcept = delete;
    virtual ~Syntax() noexcept = default;

    //--- public operators ---
    Syntax &operator=(const Syntax &rhs) noexcept(false) = delete;
    Syntax &operator=(Syntax &&rhs) noexcept = delete;

    //--- public methods ---
    static const QString toString(const Types::Syntax syntax) noexcept;
    virtual Types::Syntax type() const noexcept = 0;
    virtual const QString name() const noexcept = 0;
};
