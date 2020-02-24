#pragma once

#include <QTextDocument>
#include <QWidget>

namespace Ui { class SearchWindow; }
class QEvent;

class SearchWindow : public QWidget {
    Q_OBJECT
public:
    //--- public properties ---
    Ui::SearchWindow *ui;

    //--- public constructors ---
    SearchWindow(QWidget *parent = nullptr);
    SearchWindow(const SearchWindow &rhs) = delete;
    SearchWindow(SearchWindow &&rhs) = delete;
    ~SearchWindow();

    //--- public operators ---
    SearchWindow &operator=(const SearchWindow &rhs) = delete;
    SearchWindow &operator=(SearchWindow &&rhs) = delete;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;

    void setupActions();

signals:
    //--- own signals ---
    void searchRequest(const QString &str, const QTextDocument::FindFlags flags,
        const bool use_regexp);
};
