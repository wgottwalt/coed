#pragma once

#include <QMainWindow>
#include <QPalette>
#include <QString>
#include "Types.hxx"

namespace Ui { class MainWindow; }
class QCloseEvent;
class QEvent;
class QSettings;
class QTranslator;
class EditorWindow;
class SearchWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    //--- public properties ---
    const QPalette OldPalette;
    Ui::MainWindow *ui;
    SearchWindow *search;

    //--- public constructors ---
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const MainWindow &rhs) = delete;
    MainWindow(MainWindow &&rhs) = delete;
    virtual ~MainWindow();

    //--- public operators ---
    MainWindow &operator=(const MainWindow &rhs) = delete;
    MainWindow &operator=(MainWindow &&rhs) = delete;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    virtual void closeEvent(QCloseEvent *event) override final;

    void setupActions();
    void setupToolbar();
    void setLanguage(const Types::Language lang = Types::Language::English);
    void setTheme(const Types::Theme theme = Types::Theme::Fusion);
    void setDarkmode(const bool mode);

private:
    //--- private properties ---
    QTranslator *_trans;
    QSettings *_conf;
    Types::Language _lang;
    Types::Theme _theme;
    bool _darkmode;

    //--- private methods ---
    EditorWindow *createEditor(const QString &filename = "");
    EditorWindow *currentEditor();
};
