#pragma once

#include <QSyntaxHighlighter>

#include "Types.hxx"

class QTextDocument;

class Syntax : public QSyntaxHighlighter {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit Syntax(QTextDocument *parent = nullptr);
    Syntax(const Syntax &rhs) = delete;
    Syntax(Syntax &&rhs) noexcept = delete;
    ~Syntax() noexcept override = default;

    //--- public operators ---
    Syntax &operator=(const Syntax &rhs) = delete;
    Syntax &operator=(Syntax &&rhs) noexcept = delete;

    //--- public methods ---
    static QString toString(Types::Syntax syntax) noexcept;
    [[nodiscard]] virtual Types::Syntax type() const noexcept = 0;
    [[nodiscard]] virtual QString name() const noexcept = 0;
};
