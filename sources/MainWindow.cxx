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
#include "EditorWindow.hxx"
#include "SearchWindow.hxx"

//--- public constructors ---

MainWindow::MainWindow(QWidget *parent) noexcept(false)
: QMainWindow(parent), Ui::MainWindow(), _trans(new QTranslator(this)),
  _conf(new QSettings(qApp->organizationName(), qApp->applicationName(), this)),
  _search(new SearchWindow), _old_palette(qApp->palette()), _lang(Types::Language::English),
  _theme(Types::Theme::Fusion), _darkmode(false)
{
    setupUi(this);
    _search->setWindowFlags(Qt::WindowStaysOnTopHint);
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

    me_theme_fusion->setVisible(false);
    me_theme_qtcurve->setVisible(false);
    me_theme_windows->setVisible(false);

    for (auto &stylename : QStyleFactory::keys())
    {
        if (stylename == "Fusion")
            me_theme_fusion->setVisible(true);

        if (stylename == "QtCurve")
            me_theme_qtcurve->setVisible(true);

        if (stylename == "Windows")
            me_theme_windows->setVisible(true);
    }
}

MainWindow::~MainWindow() noexcept
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

    delete _search;
    delete _conf;
    delete _trans;
}

//--- protected methods and Qt slots ---

void MainWindow::changeEvent(QEvent *event) noexcept(false)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);

    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) noexcept(false)
{
    _search->close();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupActions() noexcept(false)
{
    // file menu
    connect(mm_file, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        //me_file_new->setEnabled(true);
        //me_file_open->setEnabled(true);
        me_file_save->setEnabled((editor && editor->filenameIsSet() &&
            editor->textHasChanged()) ? true : false);
        me_file_saveas->setEnabled(editor ? true : false);
        //me_file_exit->setEnabled(true);
    });
    connect(me_file_new, &QAction::triggered, [&](){ createEditor(); });
    connect(me_file_open, &QAction::triggered, [&]()
    {
        if (auto filenames = QFileDialog::getOpenFileNames(this, tr("I18N_FILEOPEN"), "./",
            tr("I18N_FILEOPEN_FILTER")); filenames.size())
        {
            for (auto &filename : filenames)
                createEditor(filename);
        }
    });
    connect(me_file_save, &QAction::triggered, [&]()
    {
        auto *editor = currentEditor();

        if (editor && editor->filenameIsSet())
            editor->saveFile(editor->filename());
    });
    connect(me_file_saveas, &QAction::triggered, [&]()
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
    connect(me_file_exit, &QAction::triggered, this, &MainWindow::close);

    // edit menu
    connect(mm_edit, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        me_edit_undo->setEnabled((editor && editor->undoIsReady()) ? true : false);
        me_edit_redo->setEnabled((editor && editor->redoIsReady()) ? true : false);
        me_edit_cut->setEnabled((editor &&
            editor->textCursor().selectedText().size()) ? true : false);
        me_edit_copy->setEnabled((editor &&
            editor->textCursor().selectedText().size()) ? true : false);
        me_edit_paste->setEnabled(editor ? true : false);
        me_edit_clear->setEnabled((editor &&
            editor->textCursor().selectedText().size()) ? true : false);
        me_edit_selectall->setEnabled(editor ? true : false);
    });
    connect(me_edit_undo, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Undo);
    });
    connect(me_edit_redo, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Redo);
    });
    connect(me_edit_cut, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Cut);
    });
    connect(me_edit_copy, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Copy);
    });
    connect(me_edit_paste, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Paste);
    });
    connect(me_edit_clear, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::Delete);
    });
    connect(me_edit_selectall, &QAction::triggered, [&]()
    {
        if (auto *editor = currentEditor(); editor)
            editor->doTextAction(Types::TextAction::SelectAll);
    });

    // search menu
    connect(mm_search, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        me_search_find->setEnabled(editor ? true : false);
        me_search_next->setEnabled(editor ? true : false);
        me_search_prev->setEnabled(editor ? true : false);
    });
    connect(me_search_find, &QAction::triggered, [&](){ _search->show(); });

    // window menu
    connect(mm_window, &QMenu::aboutToShow, [&]()
    {
        auto *editor = currentEditor();

        me_window_next->setEnabled((editor &&
            !wid_mdi->subWindowList().isEmpty()) ? true : false);
        me_window_prev->setEnabled((editor &&
            !wid_mdi->subWindowList().isEmpty()) ? true : false);
        me_window_tile->setEnabled(editor ? true : false);
        me_window_cascade->setEnabled(editor ? true : false);
        me_window_minimize->setEnabled(editor ? true : false);
        me_window_maximize->setEnabled(editor ? true : false);
        me_window_close->setEnabled(editor ? true : false);
        me_window_closeall->setEnabled(editor ? true : false);
    });
    connect(me_window_next, &QAction::triggered, [&](){ wid_mdi->activateNextSubWindow(); });
    connect(me_window_prev, &QAction::triggered, [&](){ wid_mdi->activatePreviousSubWindow(); });
    connect(me_window_tile, &QAction::triggered, [&](){ wid_mdi->tileSubWindows(); });
    connect(me_window_cascade, &QAction::triggered, [&](){ wid_mdi->cascadeSubWindows(); });
    connect(me_window_maximize, &QAction::triggered, [&]()
    {
        if (auto list = wid_mdi->subWindowList(); !list.isEmpty())
            for (auto *subwin : list)
                subwin->showNormal();
    });
    connect(me_window_minimize, &QAction::triggered, [&]()
    {
        if (auto list = wid_mdi->subWindowList(); !list.isEmpty())
            for (auto *subwin : list)
                subwin->showShaded();
    });
    connect(me_window_close, &QAction::triggered, [&](){ wid_mdi->closeActiveSubWindow(); });
    connect(me_window_closeall, &QAction::triggered, [&](){ wid_mdi->closeAllSubWindows(); });

    // config menu
    connect(me_lang_english, &QAction::triggered, [&](){ setLanguage(); });
    connect(me_lang_dutch, &QAction::triggered, [&](){ setLanguage(Types::Language::Dutch); });
    connect(me_lang_french, &QAction::triggered, [&](){ setLanguage(Types::Language::French); });
    connect(me_lang_german, &QAction::triggered, [&](){ setLanguage(Types::Language::German); });
    connect(me_lang_italian, &QAction::triggered, [&](){ setLanguage(Types::Language::Italian); });
    connect(me_lang_spanish, &QAction::triggered, [&](){ setLanguage(Types::Language::Spanish); });

    connect(me_theme_fusion, &QAction::triggered, [&](){ setTheme(); });
    connect(me_theme_qtcurve, &QAction::triggered, [&](){ setTheme(Types::Theme::QtCurve); });
    connect(me_theme_windows, &QAction::triggered, [&](){ setTheme(Types::Theme::Windows); });
    connect(me_theme_darkmode, &QAction::toggled, [&](const bool on){ setDarkmode(on); });

    // about menu
    connect(me_about_qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // find dialog
    connect(_search, &SearchWindow::searchRequest, [&](const QString &str,
        const QTextDocument::FindFlags flags, const bool use_regexp)
    {
        if (auto *editor = currentEditor(); editor)
            editor->search(str, flags, use_regexp);
    });
    connect(wid_mdi, &QMdiArea::subWindowActivated, [&](QMdiSubWindow *window)
    {
        if (window)
            _search->setSearchString(qobject_cast<EditorWindow *>(
                window->widget())->searchString());
        else
            _search->hide();
    });
}

void MainWindow::setupToolbar() noexcept
{
}

void MainWindow::setLanguage(const Types::Language lang) noexcept(false)
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
            me_lang_dutch->setEnabled(true);
            me_lang_english->setEnabled(true);
            me_lang_french->setEnabled(true);
            me_lang_german->setEnabled(true);
            me_lang_italian->setEnabled(true);
            me_lang_spanish->setEnabled(true);

            switch (lang)
            {
                case Types::Language::Dutch:
                    me_lang_dutch->setEnabled(false);
                    break;

                case Types::Language::French:
                    me_lang_french->setEnabled(false);
                    break;

                case Types::Language::German:
                    me_lang_german->setEnabled(false);
                    break;

                case Types::Language::Italian:
                    me_lang_italian->setEnabled(false);
                    break;

                case Types::Language::Spanish:
                    me_lang_spanish->setEnabled(false);
                    break;

                case Types::Language::English:
                default:
                    me_lang_english->setEnabled(false);
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

void MainWindow::setTheme(const Types::Theme theme) noexcept(false)
{
    me_theme_fusion->setEnabled(true);
    me_theme_qtcurve->setEnabled(true);
    me_theme_windows->setEnabled(true);
    _theme = theme;

    switch (theme)
    {
        case Types::Theme::QtCurve:
            qApp->setStyle(QStyleFactory::create("QtCurve"));
            me_theme_qtcurve->setEnabled(false);
            break;

        case Types::Theme::Windows:
            qApp->setStyle(QStyleFactory::create("Windows"));
            me_theme_windows->setEnabled(false);
            break;

        case Types::Theme::Fusion:
        default:
            qApp->setStyle(QStyleFactory::create("Fusion"));
            me_theme_fusion->setEnabled(false);
    }
}

void MainWindow::setDarkmode(const bool mode) noexcept(false)
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
        qApp->setPalette(_old_palette);

    _darkmode = mode;
    me_theme_darkmode->setChecked(mode);
}

//--- private methods ---

EditorWindow *MainWindow::createEditor(const QString &filename) noexcept(false)
{
    QMdiSubWindow *subwin = wid_mdi->addSubWindow(new EditorWindow);
    EditorWindow *editor = qobject_cast<EditorWindow *>(subwin->widget());
    QAction *action = mm_window->addAction(editor->windowTitle(), subwin, [this, subwin]()
    {
        wid_mdi->setActiveSubWindow(subwin);
    });

    connect(editor, &EditorWindow::windowTitleChanged, action, &QAction::setText);
    connect(editor, &EditorWindow::destroyed, [this, action](){ mm_window->removeAction(action); });

    if (filename.size() && !editor->openFile(filename))
    {
        wid_mdi->removeSubWindow(subwin);
        return nullptr;
    }
    subwin->setGeometry(subwin->x(), subwin->y(), width() / 2, height() * 2 / 3);
    editor->show();

    return editor;
}

EditorWindow *MainWindow::currentEditor() noexcept(false)
{
    if (QMdiSubWindow *subwin = wid_mdi->activeSubWindow(); subwin)
        return qobject_cast<EditorWindow *>(subwin->widget());
    return nullptr;
}
