#include <QEvent>
#include "SearchWindow.hxx"
#include "ui_SearchWindow.h"

//--- public constructors ---

SearchWindow::SearchWindow(QWidget *parent)
: QWidget(parent), ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    ui->lay_main->setSizeConstraint(QLayout::SetFixedSize);

    setupActions();
}

SearchWindow::~SearchWindow()
{
    delete ui;
}

//--- protected methods ---

void SearchWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);

    QWidget::changeEvent(event);
}

void SearchWindow::setupActions()
{
    connect(ui->btn_close, &QPushButton::clicked, this, &SearchWindow::close);
    connect(ui->btn_prev, &QPushButton::clicked, [&]()
    {
        QTextDocument::FindFlags flags = QTextDocument::FindBackward;

        if (ui->chk_case_sensitive->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (ui->chk_whole_words->isChecked())
            flags |= QTextDocument::FindWholeWords;

        emit searchRequest(ui->wid_lineedit->text(), flags, false);
    });

    connect(ui->btn_next, &QPushButton::clicked, [&]()
    {
        QTextDocument::FindFlags flags;

        if (ui->chk_case_sensitive->isChecked())
            flags |= QTextDocument::FindCaseSensitively;
        if (ui->chk_whole_words->isChecked())
            flags |= QTextDocument::FindWholeWords;

        emit searchRequest(ui->wid_lineedit->text(), flags, false);
    });
}
