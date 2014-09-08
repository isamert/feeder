#include "rssreader.h"

RssReader::RssReader(QObject *parent) :
    QObject(parent)
{
    this->document = new QDomDocument();
}

RssReader::~RssReader() {
    delete this->document;
}

bool RssReader::setDocument(const QString &xmlFilePath) {
    QFile file(QDir::cleanPath(xmlFilePath));

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    return this->document->setContent(&file);
    //return this->setDocumentString(QString::fromLocal8Bit(file.readAll()));
}

bool RssReader::setDocumentString(const QString &strDoc) {
    QString err;
    if(this->document->setContent(strDoc, false, &err))
        return true;

    qDebug() << err;
    return false;
}

RssReader::ChannelInfo RssReader::getChannelInfo() {
    ChannelInfo info;
    QDomElement element = this->document->documentElement();

    info.title = element.elementsByTagName("title").item(0).firstChild().nodeValue().trimmed();
    info.link = element.elementsByTagName("link").item(0).firstChild().nodeValue().trimmed();
    info.language = element.elementsByTagName("language").item(0).firstChild().nodeValue().trimmed();
    info.description = element.elementsByTagName("description").item(0).firstChild().nodeValue().trimmed();
    info.copyright = element.elementsByTagName("copyright").item(0).firstChild().nodeValue().trimmed();
    info.category = element.elementsByTagName("category").item(0).firstChild().nodeValue().trimmed();

    return info;
}

QList<RssReader::RssItem> RssReader::getRssItems() {
    QList<RssItem> itemList;

    QDomElement element = this->document->documentElement();
    QDomNodeList nodeList = element.elementsByTagName("item");

    for(int i = 0; i < nodeList.length(); ++i) {
        RssItem rssItem;
        QDomNode node = nodeList.item(i);
        QDomElement childElement = node.toElement();

        /* sometimes descriptions has some html, so we need to fect that html with other texts */
        QString description;
        QTextStream tsDescription(&description);
        childElement.elementsByTagName("description").at(0).save(tsDescription, 0);

        rssItem.title = childElement.elementsByTagName("title").at(0).firstChild().nodeValue().trimmed();
        rssItem.link = childElement.elementsByTagName("link").at(0).firstChild().nodeValue().trimmed();
        rssItem.description = description.replace("<description>", "").replace("</description>", "").trimmed();
        rssItem.pubDate = childElement.elementsByTagName("pubDate").at(0).firstChild().nodeValue().trimmed();


        itemList.append(rssItem);
    }

    return itemList;
}

QList<RssReader::RssImage> RssReader::getRssImages() {
    QList<RssImage> imageList;

    QDomElement element = this->document->documentElement();
    QDomNodeList nodeList = element.elementsByTagName("image");

    for(int i = 0; i < nodeList.length(); ++i) {
        RssImage rssImage;
        QDomNode node = nodeList.item(i);
        QDomElement childElement = node.toElement();

        rssImage.url = childElement.elementsByTagName("url").at(0).firstChild().nodeValue().trimmed();
        rssImage.link = childElement.elementsByTagName("link").at(0).firstChild().nodeValue().trimmed();
        rssImage.title = childElement.elementsByTagName("title").at(0).firstChild().nodeValue().trimmed();
        rssImage.description = childElement.elementsByTagName("description").at(0).firstChild().nodeValue().trimmed();


        imageList.append(rssImage);
    }

    return imageList;
}

int RssReader::totalCount() {
    QDomElement element = this->document->documentElement();
    QDomNodeList nodeList = element.elementsByTagName("item");
    return nodeList.length();
}
