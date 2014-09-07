#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    QIntValidator *valid = new QIntValidator(0, 100, this);

    ui->lineLimit->setValidator(valid);
    ui->lineLimit->setText("10");
    ui->lineUpdateInvertal->setValidator(valid);

    this->fs = new FeedSource();
    connect(fs, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(loadFile(QString)));
    connect(fs, SIGNAL(iconDownloadCompleted(QString)), ui->lineIcon, SLOT(setText(QString)));

    this->loadSettings();
}

SettingsDialog::~SettingsDialog()
{
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
    foreach (QString feed, set.childKeys()) {
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

    set.beginGroup("Feed_" + group);
    set.setValue("url", ui->lineAdress->text());
    set.setValue("title", ui->lineTitle->text());
    set.setValue("type", this->currentType);
    set.setValue("cache", this->currentCache);
    set.setValue("category", ui->comboCategories->currentText());
    set.setValue("limit", ui->lineLimit->text());
    set.setValue("notifications", ui->checkNotifications->isChecked());
    set.setValue("icon", ui->lineIcon->text());
    set.endGroup();

    ui->lineAdress->clear();
    ui->lineTitle->clear();
    ui->lineLimit->setText("10");
    ui->checkNotifications->setChecked(false);
    ui->lineIcon->clear();
    this->currentCache = "";
    this->currentType = "";

    QMessageBox::information(this, trUtf8("Feed Added"), trUtf8("Your feed added successfully"));
    emit this->reloadRequested();
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
    QSettings set;
    set.beginGroup("General");
    set.setValue("invertal", ui->lineUpdateInvertal->text());
    set.setValue("maxlength", ui->lineMaxLength->text());
    set.endGroup();
}

void SettingsDialog::on_btnDeleteFeed_clicked() {
    QListWidgetItem *curr = ui->listFeeds->currentItem();
    QString feedName = curr->data(Qt::UserRole).toString();

    QSettings set;
    set.remove("Feed_" + feedName);
    set.beginGroup("Feeds");
    set.remove(feedName);
    set.endGroup();

    this->loadSettings();
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
