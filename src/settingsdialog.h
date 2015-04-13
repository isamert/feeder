#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QApplication>

#include "feedsource.h"
#include "rssreader.h"
#include "atomreader.h"
#include "opml.h"

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

    FeedSource *fs_updater;
    QString currentEditCache;
    QString currentEditName;
    QString currentEditFeedUri;

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void loadSettings();
    void saveFeedOrder();
    void openEditFeedTab(const QString &feedName);

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
    void on_btnImportOpml_clicked();

    void showAddDialog();

    void on_btnUpdateFeed_clicked();
    void on_btnEditFeed_clicked();

    void on_btnEditLoad_clicked();

public slots:
    void loadFile(const QString &filePath);
    void updateFeedText(const QString &filePath);

signals:
    void reloadFromCacheRequested();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SETTINGSDIALOG_H
