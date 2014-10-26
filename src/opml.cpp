#include "opml.h"

Opml::Opml(QObject *parent) :
    QObject(parent)
{
    this->fs = new FeedSource();
    connect(fs, SIGNAL(feedDownloadCompleted(QString)), this, SLOT(loadFile(QString)));
    connect(fs, SIGNAL(iconDownloadCompleted(QString)), ui->lineIcon, SLOT(setText(QString)));
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

                //General::addFeed(xmlUrl, text, )
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
                }
            }
        }
    }
}

void Opml::exportOpml(const QString &filePath) {
    //TODO: export opml
}
