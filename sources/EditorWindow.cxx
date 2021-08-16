#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QPainter>
#include "EditorWindow.hxx"
#include "SyntaxCXX.hxx"

//--- internal stuff ---

static quint32 __counter = 0;

//--- LineNumberArea ---------------------------------------------------------//
//--- public constructor ---

LineNumberArea::LineNumberArea(EditorArea *editor) noexcept(false)
: QWidget{editor}, _editor{editor}
{
    setAttribute(Qt::WA_DeleteOnClose);
}

//--- public methods ---

QSize LineNumberArea::sizeHint() const noexcept
{
    return { _editor->lineNumberAreaWidth(), 0 };
}

//--- protected methods ---

void LineNumberArea::paintEvent(QPaintEvent *event) noexcept(false)
{
    _editor->lineNumberAreaPaintEvent(event);
}

//--- EditorArea -------------------------------------------------------------//
//--- public constructors ---

EditorArea::EditorArea(QWidget *parent) noexcept(false)
: QPlainTextEdit{parent}, ui_lna{new LineNumberArea{this}}, _font{QFont{"Fira Code", 12}},
  _fontw{QFontMetrics{_font}.horizontalAdvance(QLatin1Char{'X'})},
  _fonth{QFontMetrics{_font}.height()}, _tabwidth{8}, _use_smartfont{true}, _use_wordwrap{true},
  _show_linenumbers{true}, _show_linehighlight{true}, _tabs_to_spaces{false}
{
    setAttribute(Qt::WA_DeleteOnClose);
    document()->setDefaultFont(_font);

    connect(this, &EditorArea::blockCountChanged, this, &EditorArea::updateLineNumberAreaWidth);
    connect(this, &EditorArea::updateRequest, this, &EditorArea::updateLineNumberArea);
    connect(this, &EditorArea::cursorPositionChanged, this, &EditorArea::updateLineHighlight);

    updateLineNumberAreaWidth();
    updateLineHighlight();
}

EditorArea::~EditorArea() noexcept
{
    delete ui_lna;
}

//--- public methods ---

void EditorArea::setSmartFont(const bool on) noexcept(false)
{
    if (_use_smartfont = on; on)
        _font = {"Fira Code", 12};
    else
        _font = {"Fira Mono", 12};
    _fontw = QFontMetrics{_font}.horizontalAdvance(QLatin1Char('X'));
    _fonth = QFontMetrics{_font}.height();
    document()->setDefaultFont(_font);
    setTabStopDistance(_fontw * _tabwidth);
    setFocus();

    updateLineNumberAreaWidth();
    updateLineHighlight();
}

bool EditorArea::smartFont() const noexcept
{
    return _use_smartfont;
}

void EditorArea::setWordWrap(const bool on) noexcept
{
    if (_use_wordwrap = on; on)
        setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        setLineWrapMode(QPlainTextEdit::NoWrap);
    setFocus();
}

bool EditorArea::wordWrap() const noexcept
{
    return _use_wordwrap;
}

void EditorArea::setShowLineNumbers(const bool on) noexcept
{
    _show_linenumbers = on;
    updateLineNumberAreaWidth();
    setFocus();
}

bool EditorArea::showLineNumbers() const noexcept
{
    return _show_linenumbers;
}

void EditorArea::setShowLineHighlight(const bool on) noexcept
{
    _show_linehighlight = on;
    updateLineHighlight();
    setFocus();
}

bool EditorArea::showLineHighlight() const noexcept
{
    return _show_linehighlight;
}

void EditorArea::setTabsToSpaces(const bool on) noexcept
{
    _tabs_to_spaces = on;
    setFocus();
}

bool EditorArea::tabsToSpaces() const noexcept
{
    return _tabs_to_spaces;
}

void EditorArea::setTabWidth(const qint32 value) noexcept
{
    _tabwidth = value;
    setTabStopDistance(_fontw * _tabwidth);
    setFocus();
}

qint32 EditorArea::tabWidth() const noexcept
{
    return _tabwidth;
}

//--- protected methods ---

void EditorArea::changeEvent(QEvent *event) noexcept(false)
{
    if (event->type() == QEvent::PaletteChange)
        updateLineHighlight();

    QPlainTextEdit::changeEvent(event);
}

void EditorArea::resizeEvent(QResizeEvent *event) noexcept(false)
{
    QRect rect;

    QPlainTextEdit::resizeEvent(event);
    rect = contentsRect();
    ui_lna->setGeometry({rect.left(), rect.top(), lineNumberAreaWidth(), rect.height()});
}

void EditorArea::keyPressEvent(QKeyEvent *event) noexcept(false)
{
    if (event->key() == Qt::Key_Tab)
    {
        if (_tabs_to_spaces)
        {
            textCursor().insertText(QString(_tabwidth, ' '));
            return;
        }
    }
    QPlainTextEdit::keyPressEvent(event);
}

qint32 EditorArea::lineNumberAreaWidth() const noexcept
{
    if (_show_linenumbers)
    {
        qint32 numbers = 1;
        qint32 tmp = qMax(1, blockCount());

        while (tmp >= 10)
        {
            tmp /= 10;
            ++numbers;
        }

        return _fontw * numbers + 2;
    }

    return 0;
}

void EditorArea::lineNumberAreaPaintEvent(QPaintEvent *event) noexcept(false)
{
    if (_show_linenumbers)
    {
        const QRect rect = {event->rect().left(), event->rect().top(), event->rect().right(),
                            event->rect().height()};
        QPainter painter(ui_lna);
        QTextBlock block = firstVisibleBlock();
        qint32 bnum = block.blockNumber();
        qint32 top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        qint32 bottom = top + qRound(blockBoundingRect(block).height());

        painter.fillRect(rect, palette().color(QPalette::Window));
        painter.setPen(palette().color(QPalette::Text));
        painter.setFont(_font);

        while (block.isValid() && (top <= rect.bottom()))
        {
            if (block.isVisible() && (bottom >= rect.top()))
            {
                painter.drawText(0, top, rect.width(), _fonth, Qt::AlignRight,
                    QString::number(bnum + 1));
            }

            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++bnum;
        }
    }
}

void EditorArea::updateLineNumberAreaWidth([[maybe_unused]] const qint32 new_block_count) noexcept
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorArea::updateLineNumberArea(const QRect &rect, const qint32 delta_y) noexcept(false)
{
    if (delta_y)
        ui_lna->scroll(0, delta_y);
    else
        ui_lna->update(0, rect.y(), ui_lna->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void EditorArea::updateLineHighlight() noexcept(false)
{
    if (_show_linehighlight)
    {
        QList<QTextEdit::ExtraSelection> selections;
        QTextEdit::ExtraSelection sel;

        sel.format.setBackground(qApp->palette().color(QPalette::Window));
        sel.format.setProperty(QTextFormat::FullWidthSelection, true);
        sel.cursor = textCursor();
        sel.cursor.clearSelection();
        selections += sel;

        setExtraSelections(selections);
    }
    else
        setExtraSelections(QList<QTextEdit::ExtraSelection>());
}

//--- EditorWindow -----------------------------------------------------------//
//--- public constructors ---

EditorWindow::EditorWindow(QWidget *parent)
: QWidget(parent), Ui::EditorWindow(), _filename(""), _search_string(""),
  _ui_edit(new EditorArea(this)), _syntax(nullptr), _id(__counter++), _undo_ready(false),
  _redo_ready(false), _text_changed(false), _filename_set(false)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setupUi(this);
    lay_main->addWidget(_ui_edit);

    btn_smartfont->setChecked(_ui_edit->smartFont());
    btn_wordwrap->setChecked(_ui_edit->wordWrap());
    btn_linenumbers->setChecked(_ui_edit->showLineNumbers());
    btn_linehighlight->setChecked(_ui_edit->showLineHighlight());
    btn_tab->setChecked(_ui_edit->tabsToSpaces());
    box_syntax->addItem(tr("I18N_SYNTAX_NONE"), QVariant::fromValue(Types::Syntax::None));
    box_syntax->addItem(tr("I18N_SYNTAX_CXX"), QVariant::fromValue(Types::Syntax::CXX));
    box_subsyntax->setVisible(false);

    setupActions();

    updateWindowTitle();
    updatePanelLines();
    updatePanelPosition();
}

EditorWindow::~EditorWindow()
{
    if (_syntax)
        delete _syntax;
    delete _ui_edit;
}

//--- public methods/slots ---

bool EditorWindow::openFile(const QString &filename)
{
    if (QFile ifile(filename); ifile.exists() && ifile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        _ui_edit->setPlainText(ifile.readAll());
        ifile.close();

        _text_changed = false;
        _filename_set = true;
        _filename = filename;

        box_syntax->setCurrentIndex(box_syntax->findData(QVariant::fromValue(
            detectFileType(ifile))));

        updateWindowTitle();
        updatePanelLines();
        updatePanelPosition();

        return true;
    }

    return false;
}

bool EditorWindow::saveFile(const QString &filename)
{
    if (QFile ofile(filename); ofile.open(QIODevice::WriteOnly))
    {
        ofile.write(_ui_edit->toPlainText().toUtf8());
        ofile.close();

        _text_changed = false;
        _filename_set = true;
        _filename = filename;

        updateWindowTitle();

        return true;
    }

    return false;
}

void EditorWindow::search(const QString &str, const QTextDocument::FindFlags flags,
    const bool use_regexp)
{
    _search_string = str;
    if (use_regexp)
        _ui_edit->find(QRegExp(str), flags);
    else
        _ui_edit->find(str, flags);
}

QString EditorWindow::searchString() const
{
    return _search_string;
}

QString EditorWindow::filename() const
{
    return _filename_set ? _filename : "";
}

bool EditorWindow::undoIsReady() const
{
    return _undo_ready;
}

bool EditorWindow::redoIsReady() const
{
    return _redo_ready;
}

bool EditorWindow::textHasChanged() const
{
    return _text_changed;
}

bool EditorWindow::filenameIsSet() const
{
    return _filename_set;
}

QTextCursor EditorWindow::textCursor()
{
    return _ui_edit->textCursor();
}

void EditorWindow::doTextAction(const Types::TextAction action)
{
    switch (action)
    {
        case Types::TextAction::Undo:
            _ui_edit->undo();
            return;

        case Types::TextAction::Redo:
            _ui_edit->redo();
            return;

        case Types::TextAction::Cut:
            _ui_edit->cut();
            return;

        case Types::TextAction::Copy:
            _ui_edit->copy();
            return;

        case Types::TextAction::Paste:
            _ui_edit->paste();
            return;

        case Types::TextAction::Delete:
            _ui_edit->textCursor().clearSelection();
            return;

        case Types::TextAction::SelectAll:
            _ui_edit->selectAll();
            return;
    }
}

//--- protected methods ---

void EditorWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);

    QWidget::changeEvent(event);
}

void EditorWindow::setupActions()
{
    connect(_ui_edit, &QPlainTextEdit::undoAvailable, [&](const bool a){ _undo_ready = a; });
    connect(_ui_edit, &QPlainTextEdit::redoAvailable, [&](const bool a){ _redo_ready = a; });
    connect(_ui_edit, &QPlainTextEdit::textChanged, [&]()
    {
        _text_changed = true;
        updateWindowTitle();
        updatePanelLines();
    });
    connect(_ui_edit, &QPlainTextEdit::cursorPositionChanged, [&](){ updatePanelPosition(); });

    connect(btn_smartfont, &QToolButton::clicked, _ui_edit, &EditorArea::setSmartFont);
    connect(btn_wordwrap, &QToolButton::clicked, _ui_edit, &EditorArea::setWordWrap);
    connect(btn_linenumbers, &QToolButton::clicked, _ui_edit, &EditorArea::setShowLineNumbers);
    connect(btn_linehighlight, &QToolButton::clicked, _ui_edit,
        &EditorArea::setShowLineHighlight);
    connect(btn_tab, &QToolButton::clicked, _ui_edit, &EditorArea::setTabsToSpaces);
    connect(btn_tabwidth, static_cast<void (QSpinBox::*)(qint32)>(&QSpinBox::valueChanged),
        _ui_edit, &EditorArea::setTabWidth);
    connect(box_syntax, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged),
        this, &EditorWindow::switchSyntax);
}

void EditorWindow::updateWindowTitle()
{
    setWindowTitle("(" + QString::number(_id) + ") " +
        (_filename.size() ? _filename : tr("I18N_UNKNOWN_FILE")) + (_text_changed ? "*" : ""));
}

void EditorWindow::updatePanelLines()
{
    lab_lines->setText(QString::number(_ui_edit->document()->lineCount()));
}

void EditorWindow::updatePanelPosition()
{
    const quint32 xpos = _ui_edit->textCursor().positionInBlock();
    const quint32 ypos = _ui_edit->textCursor().blockNumber();

    lab_pos->setText(QString::number(xpos) + ", " + QString::number(ypos));
}

//--- private methods ---

Types::Syntax EditorWindow::detectFileType(const QFile &file) const
{
    const QFileInfo info(file);

    if ((info.suffix() == "cc") || (info.suffix() == "cpp") || (info.suffix() == "cxx") ||
        (info.suffix() == "h") || (info.suffix() == "hpp") || (info.suffix() == "hxx"))
        return Types::Syntax::CXX;

    return Types::Syntax::None;
}

void EditorWindow::switchSyntax(const qint32 index)
{
    if (index < 0)
        return;

    box_subsyntax->setVisible(false);

    switch (box_syntax->itemData(index).value<Types::Syntax>())
    {
        case Types::Syntax::CXX:
            _syntax = new SyntaxCXX(_ui_edit->document());

            box_subsyntax->addItem(tr("I18N_CXX_STD98"),
                QVariant::fromValue(Types::CXX::STD98));
            box_subsyntax->addItem(tr("I18N_CXX_STD11"),
                QVariant::fromValue(Types::CXX::STD11));
            box_subsyntax->addItem(tr("I18N_CXX_STD14"),
                QVariant::fromValue(Types::CXX::STD14));
            box_subsyntax->addItem(tr("I18N_CXX_STD17"),
                QVariant::fromValue(Types::CXX::STD17));
            box_subsyntax->addItem(tr("I18N_CXX_STD20"),
                QVariant::fromValue(Types::CXX::STD20));
            box_subsyntax->setVisible(true);

            connect(box_subsyntax,
                static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged),
                [this](const qint32 index)
            {
                static_cast<SyntaxCXX *>(_syntax)->setStandard(
                    box_subsyntax->itemData(index).value<Types::CXX>());
                _syntax->rehighlight();
            });

            break;

        case Types::Syntax::None:
        default:
            if (_syntax)
            {
                delete _syntax;
                _syntax = nullptr;
            }
    }
}
