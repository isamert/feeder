#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>

#include "feedsource.h"
#include "rssreader.h"
#include "atomreader.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::SettingsDialog *ui;
    FeedSource *fs;
    QString currentType;
    QString currentCache;

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void loadSettings();
    void saveFeedOrder();

private slots:
    void on_lineAdress_textChanged(const QString &arg1);
    void on_lineNewCategory_textChanged(const QString &arg1);
    void on_btnAdd_clicked();
    void on_btnLoad_clicked();
    void on_btnAddNewCategory_clicked();
    void on_listCategories_itemSelectionChanged();
    void on_lineAdress_returnPressed();
    void on_btnSave_clicked();
    void on_listFeeds_itemSelectionChanged();
    void on_btnDeleteFeed_clicked();
    void on_btnDeleteCategory_clicked();
    void on_btnFeedUp_clicked();
    void on_btnFeedDown_clicked();

public slots:
    void loadFile(const QString &filePath);

signals:
    void reloadRequested();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SETTINGSDIALOG_H
