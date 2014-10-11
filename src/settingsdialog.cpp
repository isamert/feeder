#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    QIntValidator *valid = new QIntValidator(0, 100, this);

    ui->lineUpdateInvertal->setValidator(valid);
    ui->lineMaxLength->setValidator(valid);
    ui->lineLimit->setValidator(valid);
    ui->lineLimit->setText("7");

    this->fs = new FeedSource();
    connect(fs, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(loadFile(QString)));
    connect(fs, SIGNAL(iconDownloadCompleted(QString)), ui->lineIcon, SLOT(setText(QString)));

    this->loadSettings();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::loadSettings() {
    QSettings set;

    /* load categories */
    ui->comboCategories->clear();
    ui->listCategories->clear();

    set.beginGroup("Categories");
    foreach(QString cat, set.childKeys()) {
        ui->comboCategories->addItem(cat);
        ui->listCategories->addItem(cat);
    }
    set.endGroup();

    /* load feeds */
    ui->listFeeds->clear();

    set.beginGroup("Feeds");
    foreach (QString feed, General::getFeeds()) {
        QListWidgetItem *item = new QListWidgetItem(ui->listFeeds);
        item->setText(set.value(feed).toString());
        item->setData(Qt::UserRole, feed);
        ui->listFeeds->addItem(item);
    }
    set.endGroup();

    /* load general settings */
    set.beginGroup("General");
    ui->lineUpdateInvertal->setText(set.value("invertal", "30").toString());
    ui->lineMaxLength->setText(set.value("maxlength", "35").toString());
    set.endGroup();

}

void SettingsDialog::on_lineAdress_textChanged(const QString &arg1) {
    ui->btnLoad->setEnabled(!ui->lineAdress->text().isEmpty());
}

void SettingsDialog::on_lineNewCategory_textChanged(const QString &arg1) {
    ui->btnAddNewCategory->setEnabled(!ui->lineNewCategory->text().isEmpty());
}

void SettingsDialog::on_listCategories_itemSelectionChanged() {
    if(ui->listCategories->currentItem() != NULL) {
        ui->btnEditCategory->setEnabled(true);
        ui->btnDeleteCategory->setEnabled(true);
    }
}

void SettingsDialog::on_listFeeds_itemSelectionChanged() {
    if(ui->listFeeds->currentItem() != NULL) {
        ui->btnDeleteFeed->setEnabled(true);
        ui->btnFeedDown->setEnabled(true);
        ui->btnFeedUp->setEnabled(true);
    }
}

void SettingsDialog::on_btnAddNewCategory_clicked() {
    QSettings set;
    set.beginGroup("Categories");
    set.setValue(ui->lineNewCategory->text(), "");
    set.endGroup();

    ui->lineNewCategory->clear();
    this->loadSettings();
}

void SettingsDialog::on_btnAdd_clicked() {
    QSettings set;
    QString group = ui->lineTitle->text().replace(" ", "").toUpper();

    set.beginGroup("Feeds");
    set.setValue(group, ui->lineTitle->text());
    set.endGroup();

    //add to order
    set.beginGroup("FeedOrder");
    QStringList orders = set.childKeys();
    orders.sort();

    int itemOrder;
    if(orders.isEmpty())
        itemOrder = 0;
    else
        itemOrder = orders.last().toInt() + 1;

    set.setValue(QString::number(itemOrder), group);
    set.endGroup();

    set.beginGroup("Feed_" + group);
    set.setValue("url", ui->lineAdress->text());
    set.setValue("title", ui->lineTitle->text());
    set.setValue("type", this->currentType);
    set.setValue("cache", this->currentCache);
    set.setValue("category", ui->comboCategories->currentText());
    set.setValue("limit", ui->lineLimit->text());
    set.setValue("notifications", ui->checkNotifications->isChecked());
    set.setValue("submenu", ui->checkSubmenu->isChecked());
    set.setValue("icon", ui->lineIcon->text());
    set.endGroup();

    ui->lineAdress->clear();
    ui->lineTitle->clear();
    ui->lineLimit->setText("7");
    ui->checkNotifications->setChecked(false);
    ui->checkSubmenu->setChecked(false);
    ui->lineIcon->clear();
    ui->btnAdd->setEnabled(false);
    this->currentCache = "";
    this->currentType = "";

    QMessageBox::information(this, trUtf8("Feed Added"), trUtf8("Your feed added successfully"));
    this->loadSettings();
    emit this->reloadFromCacheRequested();
}


void SettingsDialog::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}


void SettingsDialog::on_lineAdress_returnPressed() {
    ui->btnLoad->click();
}

void SettingsDialog::on_btnLoad_clicked() {
    fs->downloadFeed(QUrl::fromUserInput(ui->lineAdress->text()));
    fs->downloadIcon();
}

void SettingsDialog::loadFile(const QString &filePath) {
    ui->btnAdd->setEnabled(true);

    int type = FeedSource::fileType(filePath);
    this->currentCache = filePath;

    if(type == FeedSource::AtomFile) {
        AtomReader ar;
        ar.setDocument(filePath);

        AtomReader::ChannelInfo info = ar.getChannelInfo();
        ui->lineTitle->setText(info.title);
        this->currentType = "atom";
    }
    else if(type == FeedSource::RssFile) {
        RssReader rr;
        rr.setDocument(filePath);

        RssReader::ChannelInfo info = rr.getChannelInfo();
        ui->lineTitle->setText(info.title);
        this->currentType = "rss";
    }
    else {
        ui->lineTitle->setText(trUtf8("Error while loading file"));
    }
}

void SettingsDialog::on_btnSave_clicked() {
    bool startup = ui->checkStartup->isChecked();
    if(startup) {
        if(!QFile::exists(General::autostartFile())) {
            QString desktopFile = QString("[Desktop Entry]\n"
                                          "Name=%1\n"
                                          "Exec=%2\n"
                                          "Encoding=UTF-8\n"
                                          "Type=Application\n"
                                          "X-GNOME-Autostart-enabled=true\n"
                                          "Icon=%3").arg(qApp->applicationName())
                                                    .arg(qApp->applicationFilePath())
                                                    .arg(qApp->applicationName());
            QFile file(General::autostartFile());
            if(!file.open(QFile::WriteOnly))
                ui->checkStartup->setChecked(false);
            else {
                QTextStream ts(&file);
                ts << desktopFile;
            }
            file.close();
        }
    }
    else {
        if(QFile::exists(General::autostartFile()))
            QFile::remove(General::autostartFile());
    }

    QSettings set;
    set.beginGroup("General");
    set.setValue("invertal", ui->lineUpdateInvertal->text());
    set.setValue("maxlength", ui->lineMaxLength->text());
    set.setValue("startup", ui->checkStartup->isChecked());
    set.endGroup();

    emit this->reloadFromCacheRequested();
}

void SettingsDialog::on_btnDeleteFeed_clicked() {
    QListWidgetItem *curr = ui->listFeeds->currentItem();
    QString feedName = curr->data(Qt::UserRole).toString();

    //delete from Feeds section
    QSettings set;
    set.remove("Feed_" + feedName);
    set.beginGroup("Feeds");
    set.remove(feedName);
    set.endGroup();

    //delete from FeedOrder section
    set.beginGroup("FeedOrder");
    foreach (QString order, set.childKeys()) {
        if(set.value(order) == feedName)
            set.remove(order);
    }

    ui->listFeeds->setCurrentRow(0);
    this->loadSettings();

    emit this->reloadFromCacheRequested();
}

void SettingsDialog::on_btnDeleteCategory_clicked() {
    //TODO: remove all sub-feeds
    QListWidgetItem *curr = ui->listCategories->currentItem();

    QSettings set;
    set.beginGroup("Categories");
    set.remove(curr->text());
    set.endGroup();

    this->loadSettings();
}

void SettingsDialog::saveFeedOrder() {
    QSettings set;
    set.remove("FeedOrder");
    set.beginGroup("FeedOrder");
    for(int i = 0; i < ui->listFeeds->count(); ++i)
        set.setValue(QString::number(i), ui->listFeeds->item(i)->data(Qt::UserRole).toString());
    set.endGroup();
}

void SettingsDialog::on_btnFeedUp_clicked() {
    int index = ui->listFeeds->currentRow();

    if(index > 0) {
        QListWidgetItem *lwi = ui->listFeeds->takeItem(index);
        ui->listFeeds->insertItem(index - 1, lwi);
        ui->listFeeds->setCurrentRow(index - 1);

        this->saveFeedOrder();
        emit this->reloadFromCacheRequested();
    }
}

void SettingsDialog::on_btnFeedDown_clicked() {
    int index = ui->listFeeds->currentRow();

    if(index < ui->listFeeds->count() - 1) {
        QListWidgetItem *lwi = ui->listFeeds->takeItem(index);
        ui->listFeeds->insertItem(index + 1, lwi);
        ui->listFeeds->setCurrentRow(index + 1);

        this->saveFeedOrder();
        emit this->reloadFromCacheRequested();
    }
}

void SettingsDialog::showAddDialog() {
    ui->tabWidget->setCurrentIndex(1);
    this->show();
}
