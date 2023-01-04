#pragma once

#include <QMainWindow>
#include <QPalette>
#include <QString>

#include "ui_MainWindow.h"
#include "Types.hxx"

class QSettings;
class QTranslator;
class EditorWindow;
class SearchWindow;

class MainWindow : public QMainWindow, protected Ui::MainWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit MainWindow(QWidget *parent = nullptr);
    MainWindow(const MainWindow &rhs) = delete;
    MainWindow(MainWindow &&rhs) noexcept = delete;
    ~MainWindow() noexcept override;

    //--- public operators ---
    MainWindow &operator=(const MainWindow &rhs) = delete;
    MainWindow &operator=(MainWindow &&rhs) noexcept = delete;

protected:
    //--- protected methods ---
    void changeEvent(QEvent *event) final;
    void closeEvent(QCloseEvent *event) final;

    void setupActions();
    void setupToolbar();
    void setLanguage(Types::Language lang = Types::Language::English);
    void setTheme(Types::Theme theme = Types::Theme::Fusion);
    void setDarkmode(bool mode);

private:
    //--- private properties ---
    QTranslator *_trans;
    QSettings *_conf;
    SearchWindow *_search;
    QPalette _old_palette;
    Types::Language _lang;
    Types::Theme _theme;
    bool _darkmode;

    //--- private methods ---
    EditorWindow *createEditor(const QString &filename = "");
    EditorWindow *currentEditor();
};
