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
    LineNumberArea(EditorArea *editor = nullptr);
    LineNumberArea(const LineNumberArea &rhs) = delete;
    LineNumberArea(LineNumberArea &&rhs) = delete;
    virtual ~LineNumberArea();

    //--- public operators ---
    LineNumberArea &operator=(const LineNumberArea &rhs) = delete;
    LineNumberArea &operator=(LineNumberArea &&rhs) = delete;

    //--- public methods ---
    virtual QSize sizeHint() const override final;

protected:
    //--- protected methods ---
    virtual void paintEvent(QPaintEvent *event) override final;

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
    EditorArea(QWidget *parent = nullptr);
    EditorArea(const EditorArea &rhs) = delete;
    EditorArea(EditorArea &&rhs) = delete;
    virtual ~EditorArea();

    //--- public operators ---
    EditorArea &operator=(const EditorArea &rhs) = delete;
    EditorArea &operator=(EditorArea &&rhs) = delete;

    //--- public methods ---
    void setSearchString(const QString &str);
    QString searchString() const;
    void setSmartFont(const bool on = true);
    bool smartFont() const;
    void setWordWrap(const bool on = true);
    bool wordWrap() const;
    void setShowLineNumbers(const bool on = true);
    bool showLineNumbers() const;
    void setShowLineHighlight(const bool on = true);
    bool showLineHighlight() const;
    void setTabsToSpaces(const bool on = false);
    bool tabsToSpaces() const;
    void setTabWidth(const qint32 value = 8);
    qint32 tabWidth() const;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    virtual void resizeEvent(QResizeEvent *event) override final;
    virtual void keyPressEvent(QKeyEvent *event) override final;

    qint32 lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void updateLineNumberAreaWidth(const qint32 new_block_count = 0);
    void updateLineNumberArea(const QRect &rect, const qint32 delta_y);
    void updateLineHighlight();

private:
    //--- private properties ---
    LineNumberArea *ui_lna;
    QString _search_string;
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
    //--- public properties ---
    EditorArea *ui_edit;
    const quint32 id;
    bool undo_available;
    bool redo_available;
    bool text_changed;
    bool filename_set;

    //--- public constructors ---
    EditorWindow(QWidget *parent = nullptr);
    EditorWindow(const EditorWindow &rhs) = delete;
    EditorWindow(EditorWindow &&rhs) = delete;
    virtual ~EditorWindow();

    //--- public operators ---
    EditorWindow &operator=(const EditorWindow &rhs) = delete;
    EditorWindow &operator=(EditorWindow &&rhs) = delete;

    //--- public methods/slots ---
    QString filename() const;
    bool openFile(const QString &filename);
    bool saveFile(const QString &filename = "");
    void search(const QString &str, const QTextDocument::FindFlags flags, const bool use_regexp);

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
    QSyntaxHighlighter *_syntax;

    //--- private methods ---
    Types::Syntax detectFileType(const QFile &file) const;
    void switchSyntax(const qint32 index);
};
