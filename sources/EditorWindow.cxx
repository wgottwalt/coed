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

LineNumberArea::LineNumberArea(EditorArea *editor)
: QWidget(editor), _editor(editor)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

LineNumberArea::~LineNumberArea()
{
}

//--- public methods ---

QSize LineNumberArea::sizeHint() const
{
    return { _editor->lineNumberAreaWidth(), 0 };
}

//--- protected methods ---

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    _editor->lineNumberAreaPaintEvent(event);
}

//--- EditorArea -------------------------------------------------------------//
//--- public constructors ---

EditorArea::EditorArea(QWidget *parent)
: QPlainTextEdit(parent), ui_lna(new LineNumberArea(this)), _search_string(""),
  _font(QFont("Fira Code", 12)), _fontw(QFontMetrics(_font).horizontalAdvance(QLatin1Char('X'))),
  _fonth(QFontMetrics(_font).height()), _tabwidth(8), _use_smartfont(true), _use_wordwrap(true),
  _show_linenumbers(true), _show_linehighlight(true), _tabs_to_spaces(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    document()->setDefaultFont(_font);

    connect(this, &EditorArea::blockCountChanged, this, &EditorArea::updateLineNumberAreaWidth);
    connect(this, &EditorArea::updateRequest, this, &EditorArea::updateLineNumberArea);
    connect(this, &EditorArea::cursorPositionChanged, this, &EditorArea::updateLineHighlight);

    updateLineNumberAreaWidth();
    updateLineHighlight();
}

EditorArea::~EditorArea()
{
    delete ui_lna;
}

//--- public methods ---

void EditorArea::setSearchString(const QString &str)
{
    _search_string = str;
}

QString EditorArea::searchString() const
{
    return _search_string;
}

void EditorArea::setSmartFont(const bool on)
{
    if (_use_smartfont = on; on)
        _font = QFont("Fira Code", 12);
    else
        _font = QFont("Fira Mono", 12);
    _fontw = QFontMetrics(_font).horizontalAdvance(QLatin1Char('X'));
    _fonth = QFontMetrics(_font).height();
    document()->setDefaultFont(_font);
    setTabStopDistance(_fontw * _tabwidth);
    setFocus();

    updateLineNumberAreaWidth();
    updateLineHighlight();
}

bool EditorArea::smartFont() const
{
    return _use_smartfont;
}

void EditorArea::setWordWrap(const bool on)
{
    if (_use_wordwrap = on; on)
        setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        setLineWrapMode(QPlainTextEdit::NoWrap);
    setFocus();
}

bool EditorArea::wordWrap() const
{
    return _use_wordwrap;
}

void EditorArea::setShowLineNumbers(const bool on)
{
    _show_linenumbers = on;
    updateLineNumberAreaWidth();
    setFocus();
}

bool EditorArea::showLineNumbers() const
{
    return _show_linenumbers;
}

void EditorArea::setShowLineHighlight(const bool on)
{
    _show_linehighlight = on;
    updateLineHighlight();
    setFocus();
}

bool EditorArea::showLineHighlight() const
{
    return _show_linehighlight;
}

void EditorArea::setTabsToSpaces(const bool on)
{
    _tabs_to_spaces = on;
    setFocus();
}

bool EditorArea::tabsToSpaces() const
{
    return _tabs_to_spaces;
}

void EditorArea::setTabWidth(const qint32 value)
{
    _tabwidth = value;
    setTabStopDistance(_fontw * _tabwidth);
    setFocus();
}

qint32 EditorArea::tabWidth() const
{
    return _tabwidth;
}

//--- protected methods ---

void EditorArea::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange)
        updateLineHighlight();

    QPlainTextEdit::changeEvent(event);
}

void EditorArea::resizeEvent(QResizeEvent *event)
{
    QRect rect;

    QPlainTextEdit::resizeEvent(event);
    rect = contentsRect();
    ui_lna->setGeometry({.left = rect.left(), .top = rect.top(), .right = lineNumberAreaWidth(),
        .height = rect.height()});
}

void EditorArea::keyPressEvent(QKeyEvent *event)
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

qint32 EditorArea::lineNumberAreaWidth() const
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

void EditorArea::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (_show_linenumbers)
    {
        const QRect rect = {.left = event->rect().left(), .top = event->rect().top(),
                            .right = event->rect().right(), .height = event->rect().height()};
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

void EditorArea::updateLineNumberAreaWidth(const qint32 /* new_block_count */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorArea::updateLineNumberArea(const QRect &rect, const qint32 delta_y)
{
    if (delta_y)
        ui_lna->scroll(0, delta_y);
    else
        ui_lna->update(0, rect.y(), ui_lna->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void EditorArea::updateLineHighlight()
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
: QWidget(parent), Ui::EditorWindow(), ui_edit(new EditorArea(this)), id(__counter++),
  undo_available(false), redo_available(false), text_changed(false), filename_set(false),
  _filename(""), _syntax(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setupUi(this);
    lay_main->addWidget(ui_edit);
    btn_smartfont->setChecked(ui_edit->smartFont());
    btn_wordwrap->setChecked(ui_edit->wordWrap());
    btn_linenumbers->setChecked(ui_edit->showLineNumbers());
    btn_linehighlight->setChecked(ui_edit->showLineHighlight());
    btn_tab->setChecked(ui_edit->tabsToSpaces());
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
    delete ui_edit;
}

//--- public methods/slots ---

QString EditorWindow::filename() const
{
    return filename_set ? _filename : "";
}

bool EditorWindow::openFile(const QString &filename)
{
    if (QFile ifile(filename); ifile.exists() && ifile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui_edit->setPlainText(ifile.readAll());
        ifile.close();

        text_changed = false;
        filename_set = true;
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
        ofile.write(ui_edit->toPlainText().toUtf8());
        ofile.close();

        text_changed = false;
        filename_set = true;
        _filename = filename;

        updateWindowTitle();

        return true;
    }

    return false;
}

void EditorWindow::search(const QString &str, const QTextDocument::FindFlags flags,
    const bool use_regexp)
{
    ui_edit->setSearchString(str);
    if (use_regexp)
        ui_edit->find(QRegExp(str), flags);
    else
        ui_edit->find(str, flags);
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
    connect(ui_edit, &QPlainTextEdit::undoAvailable, [&](const bool a){ undo_available = a; });
    connect(ui_edit, &QPlainTextEdit::redoAvailable, [&](const bool a){ redo_available = a; });
    connect(ui_edit, &QPlainTextEdit::textChanged, [&]()
    {
        text_changed = true;
        updateWindowTitle();
        updatePanelLines();
    });
    connect(ui_edit, &QPlainTextEdit::cursorPositionChanged, [&](){ updatePanelPosition(); });

    connect(btn_smartfont, &QToolButton::clicked, ui_edit, &EditorArea::setSmartFont);
    connect(btn_wordwrap, &QToolButton::clicked, ui_edit, &EditorArea::setWordWrap);
    connect(btn_linenumbers, &QToolButton::clicked, ui_edit, &EditorArea::setShowLineNumbers);
    connect(btn_linehighlight, &QToolButton::clicked, ui_edit,
        &EditorArea::setShowLineHighlight);
    connect(btn_tab, &QToolButton::clicked, ui_edit, &EditorArea::setTabsToSpaces);
    connect(btn_tabwidth, static_cast<void (QSpinBox::*)(qint32)>(&QSpinBox::valueChanged),
        ui_edit, &EditorArea::setTabWidth);
    connect(box_syntax, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged),
        this, &EditorWindow::switchSyntax);
}

void EditorWindow::updateWindowTitle()
{
    setWindowTitle("(" + QString::number(id) + ") " +
        (_filename.size() ? _filename : tr("I18N_UNKNOWN_FILE")) + (text_changed ? "*" : ""));
}

void EditorWindow::updatePanelLines()
{
    lab_lines->setText(QString::number(ui_edit->document()->lineCount()));
}

void EditorWindow::updatePanelPosition()
{
    const quint32 xpos = ui_edit->textCursor().positionInBlock();
    const quint32 ypos = ui_edit->textCursor().blockNumber();

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
            _syntax = new SyntaxCXX(ui_edit->document());

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
