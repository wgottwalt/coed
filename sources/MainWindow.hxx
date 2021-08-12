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
    explicit MainWindow(QWidget *parent = nullptr) noexcept(false);
    MainWindow(const MainWindow &rhs) noexcept(false) = delete;
    MainWindow(MainWindow &&rhs) noexcept = delete;
    virtual ~MainWindow() noexcept;

    //--- public operators ---
    MainWindow &operator=(const MainWindow &rhs) noexcept(false) = delete;
    MainWindow &operator=(MainWindow &&rhs) noexcept = delete;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) noexcept(false) override final;
    virtual void closeEvent(QCloseEvent *event) noexcept(false) override final;

    void setupActions() noexcept(false);
    void setupToolbar() noexcept;
    void setLanguage(const Types::Language lang = Types::Language::English) noexcept(false);
    void setTheme(const Types::Theme theme = Types::Theme::Fusion) noexcept(false);
    void setDarkmode(const bool mode) noexcept(false);

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
    EditorWindow *createEditor(const QString &filename = "") noexcept(false);
    EditorWindow *currentEditor() noexcept(false);
};
