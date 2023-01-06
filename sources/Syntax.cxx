#include <QTextDocument>

#include "Syntax.hxx"

//--- public constructors ---

Syntax::Syntax(QTextDocument *parent)
: QSyntaxHighlighter{parent}
{
}

//--- public methods ---

QString Syntax::toString(Types::Syntax syntax) noexcept
{
    return syntaxToString(syntax);
}
