#include "atomreader.h"

AtomReader::AtomReader(QObject *parent) :
    QObject(parent)
{
    this->document = new QDomDocument();
}

bool AtomReader::setDocument(const QString &xmlFilePath) {
    QFile file(QDir::cleanPath(xmlFilePath));

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    return this->document->setContent(&file);
}

bool AtomReader::setDocumentString(const QString &strDoc) {
    QString err;
    if(this->document->setContent(strDoc, false, &err))
        return true;

    qDebug() << err;
    return false;
}

AtomReader::ChannelInfo AtomReader::getChannelInfo() {
    ChannelInfo info;
    QDomElement element = this->document->documentElement();

    info.title = element.elementsByTagName("title").item(0).firstChild().nodeValue().trimmed();
    info.subtitle = element.elementsByTagName("subtitle").item(0).firstChild().nodeValue().trimmed();
    info.updated = element.elementsByTagName("updated").item(0).firstChild().nodeValue().trimmed();
    info.rights = element.elementsByTagName("rights").item(0).firstChild().nodeValue().trimmed();
    info.link = element.elementsByTagName("link").item(0).attributes().namedItem("href").nodeValue();

    return info;
}

QList<AtomReader::AtomItem> AtomReader::getAtomEntries() {
    QList<AtomItem> itemList;

    QDomElement element = this->document->documentElement();
    QDomNodeList nodeList = element.elementsByTagName("entry");

    for(int i = 0; i < nodeList.length(); ++i) {
        AtomItem atomItem;
        QDomNode node = nodeList.item(i);
        QDomElement childElement = node.toElement();

        QString summary;
        QTextStream tsSummary(&summary);
        childElement.elementsByTagName("summary").at(0).save(tsSummary, 0);

        QString content;
        QTextStream tsContent(&content);
        childElement.elementsByTagName("content").at(0).save(tsContent, 0);

        atomItem.title = childElement.elementsByTagName("title").at(0).firstChild().nodeValue().trimmed();
        atomItem.link = childElement.elementsByTagName("link").item(0).attributes().namedItem("href").nodeValue();
        atomItem.published = childElement.elementsByTagName("published").at(0).firstChild().nodeValue().trimmed();
        atomItem.updated = childElement.elementsByTagName("updated").at(0).firstChild().nodeValue().trimmed();
        atomItem.content = content.trimmed();
        atomItem.summary = summary.trimmed();

        itemList.append(atomItem);
    }

    return itemList;
}

int AtomReader::totalCount() {
    QDomElement element = this->document->documentElement();
    QDomNodeList nodeList = element.elementsByTagName("entry");
    return nodeList.length();
}
