#include <QTextDocument>
#include "Syntax.hxx"

//--- public constructors ---

Syntax::Syntax(QTextDocument *parent) noexcept(false)
: QSyntaxHighlighter{parent}
{
}

//--- public methods ---

const QString Syntax::toString(const Types::Syntax syntax) noexcept
{
    return syntaxToString(syntax);
}
