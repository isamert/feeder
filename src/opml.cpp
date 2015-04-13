#include "opml.h"

Opml::Opml(QObject *parent) :
    QObject(parent)
{
}

bool Opml::importOpml(const QString &filePath) {
    QDomDocument document;
    QFile file(QDir::cleanPath(filePath));

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    if(!document.setContent(&file))
        return false;


    QDomNode body = document.elementsByTagName("body").item(0);
    QDomNodeList nodeList = body.childNodes();

    for(int i = 0; i < nodeList.length(); ++i) {
        QDomNode node = nodeList.item(i);
        QDomElement bodychild = node.toElement();

        if(bodychild.hasAttribute("type")) {
            QString type = bodychild.toElement().attribute("type");
            if(type == "rss" || type == "atom") {
                QString text = bodychild.toElement().attribute("text");
                QString htmlUrl = bodychild.toElement().attribute("htmlUrl");
                QString xmlUrl = bodychild.toElement().attribute("xmlUrl");
                QString url = bodychild.toElement().attribute("url");

                FeedSource *fs;
                fs = new FeedSource();

                fs->category = "";
                fs->title = text;
                fs->xmlUrl = xmlUrl;
                fs->htmlUrl = htmlUrl;
                fs->url = url;
                connect(fs, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(addFeed(QString)));
                //connect(fs, SIGNAL(iconDownloadCompleted(QString)), this, SLOT(setIcon()));
                //FIXME: cannot load icon

                fs->downloadFeed(QUrl::fromUserInput(xmlUrl));
                //fs->downloadIcon();
            }
        }
        else {
            QString category = bodychild.toElement().attribute("text");
            QDomNodeList items = bodychild.childNodes();

            for(int j = 0; j < items.length(); ++j) {
                QString type = items.item(j).toElement().attribute("type");
                if(type == "rss" || type == "atom") {
                    QString text = items.item(j).toElement().attribute("text");
                    QString htmlUrl = items.item(j).toElement().attribute("htmlUrl");
                    QString xmlUrl = items.item(j).toElement().attribute("xmlUrl");
                    QString url = items.item(j).toElement().attribute("url");

                    FeedSource *fs;
                    fs = new FeedSource();

                    fs->category = category;
                    fs->title = text.replace("/","").replace(".", "");
                    fs->xmlUrl = xmlUrl;
                    fs->htmlUrl = htmlUrl;
                    fs->url = url;
                    connect(fs, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(addFeed(QString)));
                    //connect(fs, SIGNAL(iconDownloadCompleted(QString)), this, SLOT(setIcon()));
                    //FIXME: cannot load icon

                    fs->downloadFeed(QUrl::fromUserInput(xmlUrl));
                    //fs->downloadIcon();
                }
            }
        }
    }
}

void Opml::exportOpml(const QString &filePath) {
    //TODO: export opml
}

void Opml::addFeed(const QString &xmlCacheFilePath) {
    FeedSource* fs = dynamic_cast<FeedSource*>(sender());

    QString type_;
    int type = FeedSource::fileType(xmlCacheFilePath);
    if(type == FeedSource::AtomFile)
        type_ = "atom";
    else if(type == FeedSource::RssFile)
        type_ = "rss";
    else {
        QMessageBox::warning(0, trUtf8("Cannot Add Feed"), trUtf8("Cannot add:") + fs->title);
        fs->deleteLater();
        return;
    }

    General::addFeed(fs->feedUrl.toString(), fs->title, type_, xmlCacheFilePath, fs->category, "5", false, false, "");

    fs->deleteLater();
}


