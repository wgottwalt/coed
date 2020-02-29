#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include <QWidget>
#include "MainWindow.hxx"
#include "ui_MainWindow.h"
#include "EditorWindow.hxx"
#include "ui_EditorWindow.h"
#include "SearchWindow.hxx"
#include "ui_SearchWindow.h"

//--- public constructors ---

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), OldPalette(qApp->palette()), ui(new Ui::MainWindow),
  search(new SearchWindow), _trans(new QTranslator(this)),
  _conf(new QSettings(qApp->organizationName(), qApp->applicationName(), this)),
  _lang(Types::Language::English), _theme(Types::Theme::Fusion), _darkmode(false)
{
    ui->setupUi(this);
    search->setWindowFlags(Qt::WindowStaysOnTopHint);
    setupActions();
    setupToolbar();

    if (_conf)
    {
        move(_conf->value("mainwindow_position", QPoint(40, 40)).toPoint());
        resize(_conf->value("mainwindow_size", QSize(800, 600)).toSize());
        _lang = stringToLanguage(_conf->value("language", "English").toString());
        _theme = stringToTheme(_conf->value("theme", "Fusion").toString());
        _darkmode = _conf->value("darkmode", "false").toBool();
    }
    else
    {
        move(40, 40);
        resize(800, 600);
    }
    setLanguage(_lang);
    setTheme(_theme);
    setDarkmode(_darkmode);

    ui->me_theme_fusion->setVisible(false);
    ui->me_theme_qtcurve->setVisible(false);
    ui->me_theme_windows->setVisible(false);

    for (auto &stylename : QStyleFactory::keys())
    {
        if (stylename == "Fusion")
            ui->me_theme_fusion->setVisible(true);

        if (stylename == "QtCurve")
            ui->me_theme_qtcurve->setVisible(true);

        if (stylename == "Windows")
            ui->me_theme_windows->setVisible(true);
    }
}

MainWindow::~MainWindow()
{
    if (_conf)
    {
        _conf->setValue("mainwindow_position", pos());
        _conf->setValue("mainwindow_size", size());
        _conf->setValue("language", languageToString(_lang));
        _conf->setValue("theme", themeToString(_theme));
        _conf->setValue("darkmode", _darkmode);
        _conf->sync();
    }

    delete _conf;
    delete _trans;
    delete search;
    delete ui;
}

//--- protected methods and Qt slots ---

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    search->close();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupActions()
{
    // file menu
    connect(ui->mm_file, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        //ui->me_file_new->setEnabled(true);
        //ui->me_file_open->setEnabled(true);
        ui->me_file_save->setEnabled((editor && editor->filename_set &&
            editor->text_changed) ? true : false);
        ui->me_file_saveas->setEnabled(editor ? true : false);
        //ui->me_file_exit->setEnabled(true);
    });

    connect(ui->me_file_new, &QAction::triggered, [&](){ createEditor(); });

    connect(ui->me_file_open, &QAction::triggered, [&]()
    {
        if (auto filenames = QFileDialog::getOpenFileNames(this, tr("I18N_FILEOPEN"), "./",
            tr("I18N_FILEOPEN_FILTER")); filenames.size())
        {
            for (auto &filename : filenames)
                createEditor(filename);
        }
    });

    connect(ui->me_file_save, &QAction::triggered, [&]()
    {
        auto *editor = currentEditor();

        if (editor && editor->filename_set)
            editor->saveFile(editor->filename());
    });

    connect(ui->me_file_saveas, &QAction::triggered, [&]()
    {
        auto *editor = currentEditor();

        if (editor)
        {
            if (auto filename = QFileDialog::getSaveFileName(this, tr("I18N_FILESAVE"), "./",
                tr("I18N_FILESAVE_FILTER")); filename.size())
            {
                editor->saveFile(filename);
            }
        }
    });

    connect(ui->me_file_exit, &QAction::triggered, [&]()
    {
        close();
    });

    // edit menu
    connect(ui->mm_edit, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        ui->me_edit_undo->setEnabled((editor && editor->undo_available) ? true : false);
        ui->me_edit_redo->setEnabled((editor && editor->redo_available) ? true : false);
        ui->me_edit_cut->setEnabled((editor &&
            editor->ui_edit->textCursor().selectedText().size()) ? true : false);
        ui->me_edit_copy->setEnabled((editor &&
            editor->ui_edit->textCursor().selectedText().size()) ? true : false);
        ui->me_edit_paste->setEnabled(editor ? true : false);
        ui->me_edit_clear->setEnabled((editor &&
            editor->ui_edit->textCursor().selectedText().size()) ? true : false);
        ui->me_edit_selectall->setEnabled(editor ? true : false);
    });

    connect(ui->me_edit_undo, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->undo();
    });

    connect(ui->me_edit_redo, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->redo();
    });

    connect(ui->me_edit_cut, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->cut();
    });

    connect(ui->me_edit_copy, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->copy();
    });

    connect(ui->me_edit_paste, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->paste();
    });

    connect(ui->me_edit_clear, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->textCursor().clearSelection();
    });

    connect(ui->me_edit_selectall, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->ui_edit->selectAll();
    });

    // search menu
    connect(ui->mm_search, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        ui->me_search_find->setEnabled(editor ? true : false);
        ui->me_search_next->setEnabled(editor ? true : false);
        ui->me_search_prev->setEnabled(editor ? true : false);
    });

    connect(ui->me_search_find, &QAction::triggered, [&]()
    {
        search->show();
    });

    // window menu
    connect(ui->mm_window, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        ui->me_window_next->setEnabled((editor &&
            !ui->wid_mdi->subWindowList().isEmpty()) ? true : false);
        ui->me_window_prev->setEnabled((editor &&
            !ui->wid_mdi->subWindowList().isEmpty()) ? true : false);
        ui->me_window_tile->setEnabled(editor ? true : false);
        ui->me_window_cascade->setEnabled(editor ? true : false);
        ui->me_window_minimize->setEnabled(editor ? true : false);
        ui->me_window_maximize->setEnabled(editor ? true : false);
        ui->me_window_close->setEnabled(editor ? true : false);
        ui->me_window_closeall->setEnabled(editor ? true : false);
    });

    connect(ui->me_window_next, &QAction::triggered, [&]()
    {
        ui->wid_mdi->activateNextSubWindow();
    });

    connect(ui->me_window_prev, &QAction::triggered, [&]()
    {
        ui->wid_mdi->activatePreviousSubWindow();
    });

    connect(ui->me_window_tile, &QAction::triggered, [&]()
    {
        ui->wid_mdi->tileSubWindows();
    });

    connect(ui->me_window_cascade, &QAction::triggered, [&]()
    {
        ui->wid_mdi->cascadeSubWindows();
    });

    connect(ui->me_window_maximize, &QAction::triggered, [&]()
    {
        if (auto list = ui->wid_mdi->subWindowList(); !list.isEmpty())
            for (auto *subwin : list)
                subwin->showNormal();
    });

    connect(ui->me_window_minimize, &QAction::triggered, [&]()
    {
        if (auto list = ui->wid_mdi->subWindowList(); !list.isEmpty())
            for (auto *subwin : list)
                subwin->showShaded();
    });

    connect(ui->me_window_close, &QAction::triggered, [&]()
    {
        ui->wid_mdi->closeActiveSubWindow();
    });

    connect(ui->me_window_closeall, &QAction::triggered, [&]()
    {
        ui->wid_mdi->closeAllSubWindows();
    });

    // config menu
    connect(ui->me_lang_english, &QAction::triggered, [&](){ setLanguage(); });
    connect(ui->me_lang_dutch, &QAction::triggered, [&](){ setLanguage(Types::Language::Dutch); });
    connect(ui->me_lang_french, &QAction::triggered,
        [&](){ setLanguage(Types::Language::French); });
    connect(ui->me_lang_german, &QAction::triggered,
        [&](){ setLanguage(Types::Language::German); });
    connect(ui->me_lang_italian, &QAction::triggered,
        [&](){ setLanguage(Types::Language::Italian); });
    connect(ui->me_lang_spanish, &QAction::triggered,
        [&](){ setLanguage(Types::Language::Spanish); });

    connect(ui->me_theme_fusion, &QAction::triggered, [&](){ setTheme(); });
    connect(ui->me_theme_qtcurve, &QAction::triggered, [&](){ setTheme(Types::Theme::QtCurve); });
    connect(ui->me_theme_windows, &QAction::triggered, [&](){ setTheme(Types::Theme::Windows); });
    connect(ui->me_theme_darkmode, &QAction::toggled, [&](const bool on){ setDarkmode(on); });

    // about menu
    connect(ui->me_about_qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // find dialog
    connect(search, &SearchWindow::searchRequest, [&](const QString &str,
        const QTextDocument::FindFlags flags, const bool use_regexp)
    {
        if (auto *editor = currentEditor(); editor)
            editor->search(str, flags, use_regexp);
    });
    connect(ui->wid_mdi, &QMdiArea::subWindowActivated, [&](QMdiSubWindow *window)
    {
        if (window)
            search->setSearchString(qobject_cast<EditorWindow *>(
                window->widget())->ui_edit->searchString());
        else
            search->hide();
    });
}

void MainWindow::setupToolbar()
{
}

void MainWindow::setLanguage(const Types::Language lang)
{
    QTranslator *translator = new QTranslator(this);

    if (translator)
    {
        switch (lang)
        {
            case Types::Language::Dutch:
                translator->load(":/Language/Dutch");
                break;

            case Types::Language::French:
                translator->load(":/Language/French");
                break;

            case Types::Language::German:
                translator->load(":/Language/German");
                break;

            case Types::Language::Italian:
                translator->load(":/Language/Italian");
                break;

            case Types::Language::Spanish:
                translator->load(":/Language/Spanish");
                break;

            case Types::Language::English:
            default:
                translator->load(":/Language/English");
        }

        if (!translator->isEmpty())
        {
            _lang = lang;
            ui->me_lang_dutch->setEnabled(true);
            ui->me_lang_english->setEnabled(true);
            ui->me_lang_french->setEnabled(true);
            ui->me_lang_german->setEnabled(true);
            ui->me_lang_italian->setEnabled(true);
            ui->me_lang_spanish->setEnabled(true);

            switch (lang)
            {
                case Types::Language::Dutch:
                    ui->me_lang_dutch->setEnabled(false);
                    break;

                case Types::Language::French:
                    ui->me_lang_french->setEnabled(false);
                    break;

                case Types::Language::German:
                    ui->me_lang_german->setEnabled(false);
                    break;

                case Types::Language::Italian:
                    ui->me_lang_italian->setEnabled(false);
                    break;

                case Types::Language::Spanish:
                    ui->me_lang_spanish->setEnabled(false);
                    break;

                case Types::Language::English:
                default:
                    ui->me_lang_english->setEnabled(false);
            }

            qApp->removeTranslator(_trans);
            delete _trans;
            _trans = translator;
            qApp->installTranslator(translator);
        }
        else
            delete translator;
    }
}

void MainWindow::setTheme(const Types::Theme theme)
{
    ui->me_theme_fusion->setEnabled(true);
    ui->me_theme_qtcurve->setEnabled(true);
    ui->me_theme_windows->setEnabled(true);
    _theme = theme;

    switch (theme)
    {
        case Types::Theme::QtCurve:
            qApp->setStyle(QStyleFactory::create("QtCurve"));
            ui->me_theme_qtcurve->setEnabled(false);
            break;

        case Types::Theme::Windows:
            qApp->setStyle(QStyleFactory::create("Windows"));
            ui->me_theme_windows->setEnabled(false);
            break;

        case Types::Theme::Fusion:
        default:
            qApp->setStyle(QStyleFactory::create("Fusion"));
            ui->me_theme_fusion->setEnabled(false);
    }
}

void MainWindow::setDarkmode(const bool mode)
{
    if (mode)
    {
        QPalette dark;

        dark.setColor(QPalette::Window, QColor(53, 53, 53));
        dark.setColor(QPalette::WindowText, Qt::white);
        dark.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        dark.setColor(QPalette::Base, QColor(42, 42, 42));
        dark.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        dark.setColor(QPalette::ToolTipBase, Qt::white);
        dark.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
        dark.setColor(QPalette::Text, Qt::white);
        dark.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        dark.setColor(QPalette::Dark, QColor(30, 30, 30));
        dark.setColor(QPalette::Shadow, QColor(10, 10, 10));
        dark.setColor(QPalette::Button, QColor(53, 53, 53));
        dark.setColor(QPalette::ButtonText, Qt::white);
        dark.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        dark.setColor(QPalette::BrightText, Qt::red);
        dark.setColor(QPalette::Link, QColor(42, 130, 218));
        dark.setColor(QPalette::Highlight, QColor(42, 130, 218));
        dark.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        dark.setColor(QPalette::HighlightedText, Qt::white);
        dark.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

        qApp->setPalette(dark);
    }
    else
        qApp->setPalette(OldPalette);

    _darkmode = mode;
    ui->me_theme_darkmode->setChecked(mode);
}

//--- private methods ---

EditorWindow *MainWindow::createEditor(const QString &filename)
{
    QMdiSubWindow *subwin = ui->wid_mdi->addSubWindow(new EditorWindow);
    EditorWindow *editor = qobject_cast<EditorWindow *>(subwin->widget());
    QAction *action = ui->mm_window->addAction(editor->windowTitle(), subwin, [this, subwin]()
    {
        ui->wid_mdi->setActiveSubWindow(subwin);
    });

    connect(editor, &EditorWindow::windowTitleChanged, action, &QAction::setText);
    connect(editor, &EditorWindow::destroyed, [this, action]()
    {
        ui->mm_window->removeAction(action);
    });

    if (filename.size() && !editor->openFile(filename))
    {
        ui->wid_mdi->removeSubWindow(subwin);
        return nullptr;
    }
    subwin->setGeometry(subwin->x(), subwin->y(), width() / 2, height() * 2 / 3);
    editor->show();

    return editor;
}

EditorWindow *MainWindow::currentEditor()
{
    if (QMdiSubWindow *subwin = ui->wid_mdi->activeSubWindow(); subwin)
        return qobject_cast<EditorWindow *>(subwin->widget());
    return nullptr;
}
