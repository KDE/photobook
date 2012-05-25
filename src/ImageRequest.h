/*
 *   Copyright 2012 Daniel Nicoletti <dantti12@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef IMAGEREQUEST_H
#define IMAGEREQUEST_H

#include <QThread>
#include <QFileInfo>
#include <QMetaType>
#include <QVariantList>

#include "Importer/Database.h"

class ImageFile {
public:
    bool isNull;
    QString filePath;
    int rating;
    // Xapian documment ID, null if not imported
    Xapian::Document docId;
};

class Event {
public:
    QString name;
    int photos;
    QString filePath;
    ImageFile cover;
};

class ImageRequest : public QThread
{
    Q_OBJECT
public:
    explicit ImageRequest(QObject *parent = 0);
    ~ImageRequest();

    void setRequest(const QString &request);

    static QStringList scanDirReturning(const QString &path);
    static bool isImage(const QString &filePath);

protected:
    virtual void run();

signals:
    void file(const ImageFile &fileName);
    void eventItem(const Event &event);
    
public Q_SLOTS:
    
private:
    void scanDir(const QString &path);
    static ImageFile scanFile(const QFileInfo &fileInfo);
    void getEvents();
    void getImagesFromEvent(const QString &eventName);

    QString m_request;
    Database m_db;
};

Q_DECLARE_METATYPE(ImageFile)
Q_DECLARE_METATYPE(Event)

#endif // IMAGEREQUEST_H
