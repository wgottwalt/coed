#include "SyntaxCXX.hxx"

//--- internal stuff ---

static const QString CXX98keywords{"and and_eq asm auto bitand bitor break case catch compl "
                                   "const_cast continue default delete do dynamic_cast else "
                                   "explicit export extern false for friend goto if inline "
                                   "new not not_eq operator or or_eq private protected "
                                   "public reinterpret_cast return sizeof static "
                                   "static_cast switch this throw true try "
                                   "typeid using while xor xor_eq"};
static const QString CXX11keywords{"static_assert"};
static const QString CXX20keywords{"co_await co_return co_yield"};

static const QString CXX98types{"bool char class double enum float int long namespace register "
                                "short signed struct template typedef typename union unsigned "
                                "wchar_t"};
static const QString CXX11types{"char16_t char32_t constexpr decltype thread_local"};
static const QString CXX20types{"char8_t concept requires"};

static const QString CXX98modifiers{"const mutable virtual void volatile"};
static const QString CXX11modifiers{"alignas alignof constexpr"};
static const QString CXX20modifiers{"consteval constinit"};

static const QString Prepro{"define elif else endif error if ifdef include line pragma undef "
                            "warning"};

//--- public constructors ---

SyntaxCXX::SyntaxCXX(QTextDocument *parent)
: Syntax{parent}, _keywords_format{}, _types_format{}, _modifiers_format{}, _prepro_format{},
  _slcomment_format{}, _mlcomment_format{}, _keywords_regexp{}, _types_regexp{},
  _modifiers_regexp{}, _prepro_regexp{}, _slcomment_regexp{}, _mlcomment_start_regexp{},
  _mlcomment_stop_regexp{}
{
    setStandard();
}

//--- public methods ---

Types::Syntax SyntaxCXX::type() const noexcept
{
    return Types::Syntax::CXX;
}

const QString SyntaxCXX::name() const noexcept
{
    return Syntax::toString(type());
}

void SyntaxCXX::highlightBlock(const QString &text) noexcept(false)
{
    ssize_t i = 0;

    setCurrentBlockState(0);

    // keywords
    i = text.indexOf(_keywords_regexp);
    while (i >= 0)
    {
        const size_t len = _keywords_regexp.matchedLength();

        setFormat(i, len, _keywords_format);
        i = text.indexOf(_keywords_regexp, i + len);
    }

    // types
    i = text.indexOf(_types_regexp);
    while (i >= 0)
    {
        const size_t len = _types_regexp.matchedLength();

        setFormat(i, len, _types_format);
        i = text.indexOf(_types_regexp, i + len);
    }

    // modifiers
    i = text.indexOf(_modifiers_regexp);
    while (i >= 0)
    {
        const size_t len = _modifiers_regexp.matchedLength();

        setFormat(i, len, _modifiers_format);
        i = text.indexOf(_modifiers_regexp, i + len);
    }

    // preprocessor symbols
    i = text.indexOf(_prepro_regexp);
    if (i >= 0)
        setFormat(i, _prepro_regexp.matchedLength(), _prepro_format);

    // single line comment
    i = text.indexOf(_slcomment_regexp);
    if (i >= 0)
        setFormat(i, _slcomment_regexp.matchedLength(), _slcomment_format);

    // multi line comment
    i = 0;
    if (previousBlockState() != 1)
        i = text.indexOf(_mlcomment_start_regexp);
    while (i >= 0)
    {
        ssize_t i_end = text.indexOf(_mlcomment_stop_regexp, i);
        ssize_t len = 0;

        if (i_end == -1)
        {
            setCurrentBlockState(1);
            len = text.length() - i;
        }
        else
            len = i_end - i + _mlcomment_stop_regexp.matchedLength();

        setFormat(i, len, _mlcomment_format);
        i = text.indexOf(_mlcomment_start_regexp, i + len);
    }
}

void SyntaxCXX::setStandard(const Types::CXX mode) noexcept(false)
{
    QString keyword_pattern;
    QString types_pattern;
    QString modifiers_pattern;
    QString prepro_pattern;

    _keywords_format.setFontWeight(QFont::Bold);
    _keywords_format.setForeground(Qt::darkYellow);

    _types_format.setFontWeight(QFont::Bold);
    _types_format.setForeground(Qt::darkGreen);

    _modifiers_format.setFontWeight(QFont::Normal);
    _modifiers_format.setForeground(Qt::darkGreen);

    _prepro_format.setFontWeight(QFont::Bold);
    _prepro_format.setForeground(Qt::magenta);

    _slcomment_format.setFontItalic(true);
    _slcomment_format.setForeground(Qt::darkGray);

    _mlcomment_format.setFontItalic(true);
    _mlcomment_format.setForeground(Qt::darkGray);

    keyword_pattern += "\\b(";
    types_pattern += "\\b(";
    modifiers_pattern += "\\b(";
    switch (mode)
    {
        case Types::CXX::STD20:
            keyword_pattern += CXX20keywords.split(" ").join("|") + "|";
            types_pattern += CXX20types.split(" ").join("|") + "|";
            modifiers_pattern += CXX20modifiers.split(" ").join("|") + "|";
            [[fallthrough]];

        case Types::CXX::STD17:
            [[fallthrough]];

        case Types::CXX::STD14:
            [[fallthrough]];

        case Types::CXX::STD11:
            keyword_pattern += CXX11keywords.split(" ").join("|") + "|";
            types_pattern += CXX11types.split(" ").join("|") + "|";
            modifiers_pattern += CXX11modifiers.split(" ").join("|") + "|";
            [[fallthrough]];

        case Types::CXX::STD98:
            [[fallthrough]];
        default:
            keyword_pattern += CXX98keywords.split(" ").join("|");
            types_pattern += CXX98types.split(" ").join("|");
            modifiers_pattern += CXX98modifiers.split(" ").join("|");
    }
    keyword_pattern += ")\\b";
    types_pattern += ")\\b";
    modifiers_pattern += ")\\b";

    prepro_pattern += "# *(";
    prepro_pattern += Prepro.split(" ").join("|");
    prepro_pattern += ")[^\n]*";

    _keywords_regexp.setPattern(keyword_pattern);
    _types_regexp.setPattern(types_pattern);
    _modifiers_regexp.setPattern(modifiers_pattern);
    _prepro_regexp.setPattern(prepro_pattern);
    _slcomment_regexp.setPattern("//[^\n]*");
    _mlcomment_start_regexp.setPattern("/\\*");
    _mlcomment_stop_regexp.setPattern("\\*/");
}
