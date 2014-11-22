#ifndef OPML_H
#define OPML_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QSignalMapper>
#include <QMessageBox>

#include "feedsource.h"

class Opml : public QObject
{
    Q_OBJECT
public:
    explicit Opml(QObject *parent = 0);

    bool importOpml(const QString &filePath);
    void exportOpml(const QString &filePath);

signals:

public slots:
    void addFeed(const QString &xmlCacheFilePath);

public slots:

};

#endif // OPML_H
