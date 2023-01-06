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
    explicit LineNumberArea(EditorArea *editor = nullptr);
    LineNumberArea(const LineNumberArea &rhs) = delete;
    LineNumberArea(LineNumberArea &&rhs) noexcept = delete;
    ~LineNumberArea() noexcept override = default;

    //--- public operators ---
    LineNumberArea &operator=(const LineNumberArea &rhs) = delete;
    LineNumberArea &operator=(LineNumberArea &&rhs) noexcept = delete;

    //--- public methods ---
    [[nodiscard]] QSize sizeHint() const noexcept final;

protected:
    //--- protected methods ---
    void paintEvent(QPaintEvent *event) final;

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
    explicit EditorArea(QWidget *parent = nullptr);
    EditorArea(const EditorArea &rhs) = delete;
    EditorArea(EditorArea &&rhs) noexcept = delete;
    ~EditorArea() noexcept override;

    //--- public operators ---
    EditorArea &operator=(const EditorArea &rhs) = delete;
    EditorArea &operator=(EditorArea &&rhs) noexcept = delete;

    //--- public methods ---
    void setSmartFont(bool on = true);
    [[nodiscard]] bool smartFont() const noexcept;
    void setWordWrap(bool on = true) noexcept;
    [[nodiscard]] bool wordWrap() const noexcept;
    void setShowLineNumbers(bool on = true) noexcept;
    [[nodiscard]] bool showLineNumbers() const noexcept;
    void setShowLineHighlight(bool on = true) noexcept;
    [[nodiscard]] bool showLineHighlight() const noexcept;
    void setTabsToSpaces(bool on = false) noexcept;
    [[nodiscard]] bool tabsToSpaces() const noexcept;
    void setTabWidth(qint32 value = 8) noexcept;
    [[nodiscard]] qint32 tabWidth() const noexcept;

protected:
    //--- protected methods ---
    void changeEvent(QEvent *event) final;
    void resizeEvent(QResizeEvent *event) final;
    void keyPressEvent(QKeyEvent *event) final;

    [[nodiscard]] qint32 lineNumberAreaWidth() const noexcept;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void updateLineNumberAreaWidth(qint32 new_block_count = 0) noexcept;
    void updateLineNumberArea(const QRect &rect, qint32 delta_y);
    void updateLineHighlight();

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
    explicit EditorWindow(QWidget *parent = nullptr);
    EditorWindow(const EditorWindow &rhs) = delete;
    EditorWindow(EditorWindow &&rhs) noexcept = delete;
    ~EditorWindow() noexcept override;

    //--- public operators ---
    EditorWindow &operator=(const EditorWindow &rhs) = delete;
    EditorWindow &operator=(EditorWindow &&rhs) noexcept = delete;

    //--- public methods ---
    bool openFile(const QString &filename);
    bool saveFile(const QString &filename = "");
    void search(const QString &str, QTextDocument::FindFlags flags, bool use_regexp);
    [[nodiscard]] QString searchString() const noexcept;
    [[nodiscard]] QString filename() const noexcept;
    [[nodiscard]] bool undoIsReady() const noexcept;
    [[nodiscard]] bool redoIsReady() const noexcept;
    [[nodiscard]] bool textHasChanged() const noexcept;
    [[nodiscard]] bool filenameIsSet() const noexcept;
    QTextCursor textCursor() noexcept;
    void doTextAction(Types::TextAction action) noexcept;

protected:
    //--- protected methods ---
    void changeEvent(QEvent *event) final;
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
    [[nodiscard]] Types::Syntax detectFileType(const QFile &file) const;
    void switchSyntax(qint32 index);
};
