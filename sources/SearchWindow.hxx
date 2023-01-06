#pragma once

#include <QTextDocument>
#include <QWidget>
#include "ui_SearchWindow.h"

class SearchWindow : public QWidget, protected Ui::SearchWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit SearchWindow(QWidget *parent = nullptr);
    SearchWindow(const SearchWindow &rhs) = delete;
    SearchWindow(SearchWindow &&rhs) noexcept = delete;
    ~SearchWindow() noexcept override = default;

    //--- public operators ---
    SearchWindow &operator=(const SearchWindow &rhs) = delete;
    SearchWindow &operator=(SearchWindow &&rhs) noexcept = delete;

    //--- public methods ---
    void setSearchString(const QString &str) noexcept;

protected:
    //--- protected methods ---
    void changeEvent(QEvent *event) final;
    void setupActions();

signals:
    //--- own signals ---
    void searchRequest(const QString &str, QTextDocument::FindFlags flags, bool use_regexp);
};
