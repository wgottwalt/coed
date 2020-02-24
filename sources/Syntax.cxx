#include <QTextDocument>
#include "Syntax.hxx"

//--- public constructors ---

Syntax::Syntax(QTextDocument *parent)
: QSyntaxHighlighter(parent)
{
}

Syntax::~Syntax()
{
}

//--- public methods ---

const QString Syntax::toString(const Types::Syntax syntax)
{
    return syntaxToString(syntax);
}
