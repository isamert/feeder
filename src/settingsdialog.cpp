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

    this->fs_updater = new FeedSource();
    connect(fs_updater, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(updateFeedText(QString)));
    connect(fs_updater, SIGNAL(iconDownloadCompleted(QString)), ui->lineEditIcon, SLOT(setText(QString)));

    this->loadSettings();

    ui->tab_6->setEnabled(false);
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
    ui->comboCategories->addItem(trUtf8("Empty"));
    foreach(QString cat, set.childKeys()) {
        ui->comboCategories->addItem(cat);
        ui->listCategories->addItem(cat);
    }

    ui->comboEditCategories->addItem(trUtf8("Empty"));
    foreach(QString cat, set.childKeys()) {
        ui->comboEditCategories->addItem(cat);
    }

    set.endGroup();

    /* load feeds */
    ui->listFeeds->clear();

    set.beginGroup("Feeds");
    foreach(QString feed, General::getFeeds()) {
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
    ui->checkCategories->setChecked(set.value("subcategory", false).toBool());
    QString icon = set.value("icon", ":/images/standart.png").toString();
    ui->iconBlack->setChecked(icon == ":/images/black.png");
    ui->iconWhite->setChecked(icon == ":/images/white.png");
    ui->iconStandart->setChecked(icon == ":/images/standart.png");
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
        ui->btnEditFeed->setEnabled(true);
    }
    else {
        ui->btnDeleteFeed->setEnabled(false);
        ui->btnFeedDown->setEnabled(false);
        ui->btnFeedUp->setEnabled(false);
        ui->btnEditFeed->setEnabled(false);
    }
}

void SettingsDialog::on_btnAddNewCategory_clicked() {
    General::addCategory(ui->lineNewCategory->text());
    ui->lineNewCategory->clear();
    this->loadSettings();
}

void SettingsDialog::on_btnAdd_clicked() {
    QString catname = ui->comboCategories->currentText();
    if(catname == trUtf8("Empty") || catname == "")
        catname = "";

    if(General::addFeed(ui->lineAdress->text(), ui->lineTitle->text(), this->currentType, this->currentCache, catname,
                         ui->lineLimit->text(), ui->checkNotifications->isChecked(), ui->checkSubmenu->isChecked(), ui->lineIcon->text()))
        QMessageBox::information(this, trUtf8("Feed Added"), trUtf8("Your feed added successfully"));
    else {
        QMessageBox::warning(this, trUtf8("Cannot Add Feed"), trUtf8("You have same feed already!"));
        return;
    }


    ui->lineAdress->clear();
    ui->lineTitle->clear();
    ui->lineLimit->setText("7");
    ui->checkNotifications->setChecked(false);
    ui->checkSubmenu->setChecked(false);
    ui->lineIcon->clear();
    ui->btnAdd->setEnabled(false);
    this->currentCache = "";
    this->currentType = "";

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
    else
        ui->lineTitle->setText(trUtf8("Error while loading file"));
}

void SettingsDialog::on_btnSave_clicked() {
    //TODO: add to windows startup
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
    set.setValue("subcategory", ui->checkCategories->isChecked());

    QString icon = ":/images/standart.png";
    icon = ui->iconBlack->isChecked() ? ":/images/black.png":icon;
    icon = ui->iconWhite->isChecked() ? ":/images/white.png":icon;
    set.setValue("icon", icon);
    set.endGroup();

    QMessageBox::information(0, trUtf8("Saved"), trUtf8("Settings saved successfully."));
    emit this->reloadFromCacheRequested();
}

void SettingsDialog::on_btnDeleteFeed_clicked() {
    QListWidgetItem *curr = ui->listFeeds->currentItem();
    if(curr == NULL)
        return;

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
    if(curr == NULL)
        return;

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

void SettingsDialog::on_btnImportOpml_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), "", tr("Opml files (*.opml)"));

    if(!fileName.isEmpty() && QFile::exists(fileName)) {
        Opml *opml = new Opml();
        opml->importOpml(fileName);
    }

    QMessageBox::information(0, trUtf8("Restart required"), trUtf8("Restart Feeder to see new items."));
}

void SettingsDialog::updateFeedText(const QString &filePath) {
    ui->btnAdd->setEnabled(true);

    int type = FeedSource::fileType(filePath);
    this->currentEditCache = filePath;

    if(type == FeedSource::AtomFile) {
        AtomReader ar;
        ar.setDocument(filePath);

        AtomReader::ChannelInfo info = ar.getChannelInfo();
        ui->lineEditTitle->setText(info.title);
    }
    else if(type == FeedSource::RssFile) {
        RssReader rr;
        rr.setDocument(filePath);

        RssReader::ChannelInfo info = rr.getChannelInfo();
        ui->lineEditTitle->setText(info.title);
    }
    else
        ui->lineEditTitle->setText(trUtf8("Error while loading file"));
}

void SettingsDialog::on_btnUpdateFeed_clicked() {
    QString catname = ui->comboEditCategories->currentText();
    if(catname == trUtf8("Empty") || catname == "")
        catname = "";

    QSettings set;
    set.beginGroup("Feed_" + this->currentEditName);
    set.setValue("cache", this->currentEditCache);
    set.setValue("icon", ui->lineEditIcon->text());
    set.setValue("limit", ui->lineEditLimit->text());
    set.setValue("title", ui->lineEditTitle->text());
    set.setValue("category", catname);
    set.setValue("notifications", ui->checkEditNotifications->isChecked());
    set.setValue("submenu", ui->checkEditSubmenu->isChecked());
    set.endGroup();

    emit this->reloadFromCacheRequested();

    ui->lineEditAdress->clear();
    ui->lineEditIcon->clear();
    ui->lineEditLimit->clear();
    ui->lineEditTitle->clear();
    this->currentEditCache = "";
    this->currentEditName = "";

    ui->btnUpdateFeed->setEnabled(false);
    ui->tab_6->setEnabled(false);
    ui->tabWidget->setCurrentIndex(2);
    QMessageBox::information(0, trUtf8("Feed Updated"), trUtf8("Feed information has updated."));
}

void SettingsDialog::openEditFeedTab(const QString &feedName) {
    QSettings set;
    set.beginGroup("Feed_" + feedName);
    this->currentEditName = feedName;
    this->currentEditCache = set.value("cache", "").toString();
    this->currentEditFeedUri = set.value("url", "").toString();
    ui->lineEditAdress->setText(set.value("url", "").toString());
    ui->lineEditIcon->setText(set.value("icon", "").toString());
    ui->lineEditLimit->setText(set.value("limit", "").toString());
    ui->lineEditTitle->setText(set.value("title", "").toString());
    ui->checkEditNotifications->setChecked(set.value("notifications", true).toBool());
    ui->checkEditSubmenu->setChecked(set.value("submenu", false).toBool());
    QString catname = set.value("category", "").toString();

    int count = ui->comboEditCategories->count();
    for(int i = 0; i < count; ++i) {
        QString itemtext = ui->comboEditCategories->itemText(i);
        if(itemtext == catname) {
            ui->comboEditCategories->setCurrentIndex(i);
            break;
        }
        else if(catname == "") {
            if(itemtext == trUtf8("Empty")) {
                ui->comboEditCategories->setCurrentIndex(i);
                break;
            }
        }
    }
    set.endGroup();

    ui->btnUpdateFeed->setEnabled(true);
    ui->tab_6->setEnabled(true);
    ui->tabWidget->setCurrentIndex(5);
    this->show();
}

void SettingsDialog::on_btnEditFeed_clicked() {
    QListWidgetItem *curr = ui->listFeeds->currentItem();
    if(curr == NULL)
        return;

    QString feedName = curr->data(Qt::UserRole).toString();
    this->openEditFeedTab(feedName);
}

void SettingsDialog::on_btnEditLoad_clicked() {
    this->fs_updater->downloadFeed(this->currentEditFeedUri);
    this->fs_updater->downloadIcon();
}
