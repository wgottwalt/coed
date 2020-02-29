#pragma once

#include <QTextDocument>
#include <QWidget>
#include "ui_SearchWindow.h"

class SearchWindow : public QWidget, protected Ui::SearchWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    SearchWindow(QWidget *parent = nullptr);
    SearchWindow(const SearchWindow &rhs) = delete;
    SearchWindow(SearchWindow &&rhs) = delete;
    ~SearchWindow();

    //--- public operators ---
    SearchWindow &operator=(const SearchWindow &rhs) = delete;
    SearchWindow &operator=(SearchWindow &&rhs) = delete;

    //--- public methods ---
    void setSearchString(const QString &str);

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    void setupActions();

signals:
    //--- own signals ---
    void searchRequest(const QString &str, const QTextDocument::FindFlags flags,
        const bool use_regexp);
};
