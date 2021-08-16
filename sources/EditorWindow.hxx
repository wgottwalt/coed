#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QWidget>
#include "Types.hxx"
#include "ui_EditorWindow.h"

class QEvent;
class QPaintEvent;
class QKeyEvent;
class QResizeEvent;
class QSyntaxHighlighter;
class EditorArea;
class EditorWindow;
class LineNumberArea;

//----------------------------------------------------------------------------//

class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    //--- public constructor ---
    explicit LineNumberArea(EditorArea *editor = nullptr) noexcept(false);
    LineNumberArea(const LineNumberArea &rhs) noexcept(false) = delete;
    LineNumberArea(LineNumberArea &&rhs) noexcept = delete;
    virtual ~LineNumberArea() noexcept override final = default;

    //--- public operators ---
    LineNumberArea &operator=(const LineNumberArea &rhs) noexcept(false) = delete;
    LineNumberArea &operator=(LineNumberArea &&rhs) noexcept = delete;

    //--- public methods ---
    virtual QSize sizeHint() const noexcept override final;

protected:
    //--- protected methods ---
    virtual void paintEvent(QPaintEvent *event) noexcept(false) override final;

private:
    //--- private properties ---
    EditorArea *_editor;
};

//----------------------------------------------------------------------------//

class EditorArea : public QPlainTextEdit {
    Q_OBJECT
    friend class LineNumberArea;
public:
    //--- public constructors ---
    explicit EditorArea(QWidget *parent = nullptr) noexcept(false);
    EditorArea(const EditorArea &rhs) noexcept(false) = delete;
    EditorArea(EditorArea &&rhs) noexcept = delete;
    virtual ~EditorArea() noexcept override final;

    //--- public operators ---
    EditorArea &operator=(const EditorArea &rhs) noexcept(false) = delete;
    EditorArea &operator=(EditorArea &&rhs) noexcept = delete;

    //--- public methods ---
    void setSmartFont(const bool on = true) noexcept(false);
    bool smartFont() const noexcept;
    void setWordWrap(const bool on = true) noexcept;
    bool wordWrap() const noexcept;
    void setShowLineNumbers(const bool on = true) noexcept;
    bool showLineNumbers() const noexcept;
    void setShowLineHighlight(const bool on = true) noexcept;
    bool showLineHighlight() const noexcept;
    void setTabsToSpaces(const bool on = false) noexcept;
    bool tabsToSpaces() const noexcept;
    void setTabWidth(const qint32 value = 8) noexcept;
    qint32 tabWidth() const noexcept;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) noexcept(false) override final;
    virtual void resizeEvent(QResizeEvent *event) noexcept(false) override final;
    virtual void keyPressEvent(QKeyEvent *event) noexcept(false) override final;

    qint32 lineNumberAreaWidth() const noexcept;
    void lineNumberAreaPaintEvent(QPaintEvent *event) noexcept(false);
    void updateLineNumberAreaWidth(const qint32 new_block_count = 0) noexcept;
    void updateLineNumberArea(const QRect &rect, const qint32 delta_y) noexcept(false);
    void updateLineHighlight() noexcept(false);

private:
    //--- private properties ---
    LineNumberArea *ui_lna;
    QFont _font;
    qint32 _fontw;
    qint32 _fonth;
    qint32 _tabwidth;
    bool _use_smartfont;
    bool _use_wordwrap;
    bool _show_linenumbers;
    bool _show_linehighlight;
    bool _tabs_to_spaces;
};

//----------------------------------------------------------------------------//

class EditorWindow : public QWidget, protected Ui::EditorWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    EditorWindow(QWidget *parent = nullptr);
    EditorWindow(const EditorWindow &rhs) = delete;
    EditorWindow(EditorWindow &&rhs) = delete;
    virtual ~EditorWindow();

    //--- public operators ---
    EditorWindow &operator=(const EditorWindow &rhs) = delete;
    EditorWindow &operator=(EditorWindow &&rhs) = delete;

    //--- public methods ---
    bool openFile(const QString &filename);
    bool saveFile(const QString &filename = "");
    void search(const QString &str, const QTextDocument::FindFlags flags, const bool use_regexp);
    QString searchString() const;
    QString filename() const;
    bool undoIsReady() const;
    bool redoIsReady() const;
    bool textHasChanged() const;
    bool filenameIsSet() const;
    QTextCursor textCursor();
    void doTextAction(const Types::TextAction action);

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    void setupActions();
    void updateWindowTitle();
    void updatePanelLines();
    void updatePanelPosition();

private:
    //--- private properties ---
    QString _filename;
    QString _search_string;
    EditorArea *_ui_edit;
    QSyntaxHighlighter *_syntax;
    quint32 _id;
    bool _undo_ready;
    bool _redo_ready;
    bool _text_changed;
    bool _filename_set;

    //--- private methods ---
    Types::Syntax detectFileType(const QFile &file) const;
    void switchSyntax(const qint32 index);
};
