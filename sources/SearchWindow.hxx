#pragma once

#include <QTextDocument>
#include <QWidget>
#include "ui_SearchWindow.h"

class SearchWindow : public QWidget, protected Ui::SearchWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    explicit SearchWindow(QWidget *parent = nullptr) noexcept(false);
    SearchWindow(const SearchWindow &rhs) noexcept(false) = delete;
    SearchWindow(SearchWindow &&rhs) noexcept = delete;
    virtual ~SearchWindow() noexcept = default;

    //--- public operators ---
    SearchWindow &operator=(const SearchWindow &rhs) noexcept(false) = delete;
    SearchWindow &operator=(SearchWindow &&rhs) noexcept = delete;

    //--- public methods ---
    void setSearchString(const QString &str) noexcept;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) noexcept(false) override final;
    void setupActions() noexcept(false);

signals:
    //--- own signals ---
    void searchRequest(const QString &str, const QTextDocument::FindFlags flags,
        const bool use_regexp);
};
