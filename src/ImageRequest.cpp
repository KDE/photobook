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

#include "ImageRequest.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QImageReader>

#include <KMimeType>

#include <KDebug>

ImageRequest::ImageRequest(QObject *parent) :
    QThread(parent)
{
    // Opens the DB as read only
    m_db.init(qApp->property("PicturesPath").toString(), true);

    qRegisterMetaType<ImageFile>("ImageFile");
    qRegisterMetaType<Event>("Event");
}

ImageRequest::~ImageRequest()
{
    kDebug() << m_request;
}

void ImageRequest::setRequest(const QString &request)
{
    m_request = request;
}

void ImageRequest::run()
{
    kDebug() << m_request;
    QTime t;
    t.start();
    if (m_request.startsWith(QLatin1String("events"))) {
        if (m_request == QLatin1String("events")) {
            getEvents();
        } else {
            getImagesFromEvent(m_request.mid(7));
        }
    } else if (m_request.startsWith(QLatin1Char('/'))) {
        scanDir(m_request);
    } else {
        // do xapian search

    }

    qDebug("IMAGE REQUEST Time elapsed: %d ms", t.elapsed());
    deleteLater();
}

QStringList ImageRequest::scanDirReturning(const QString &path)
{
    QStringList ret;
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        if (it.fileInfo().isFile()) {
            // Update or Insert the file on the index
            ImageFile image = scanFile(it.fileInfo());
            if (!image.isNull) {
                ret << filePath;
            }
        }
    }
    return ret;
}

bool ImageRequest::isImage(const QString &filePath)
{
    // This is way faster than checking the mime type
    // (which has to actually open the file)
    foreach (const QString &format, QImageReader::supportedImageFormats()) {
        if (filePath.endsWith(format, Qt::CaseInsensitive)) {
            return true;
        }
    }

    QImageReader image(filePath);
    // Make sure we can actually read the file
    if (image.canRead()) {
        KMimeType::Ptr mimeType;
        mimeType = KMimeType::findByFileContent(filePath);
        // Make sure the file is not a cache or something
        // that looks like an image
        if (!mimeType->is(QLatin1String("application/octet-stream"))) {
            return true;
        }
    }
    return false;
}

void ImageRequest::scanDir(const QString &path)
{
    QDir directory(path);
    foreach (const QFileInfo &fileInfo, directory.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot)) {
        if (fileInfo.isFile()) {
            // Update or Insert the file on the index
            ImageFile image = scanFile(fileInfo);
            if (!image.isNull) {
                emit file(image);
            }
        } else if (fileInfo.isDir()) {
            // Recursively search it
            scanDir(fileInfo.filePath());
        }
    }
}

// make sure fileInfo reffers to a file before calling this function
ImageFile ImageRequest::scanFile(const QFileInfo &fileInfo)
{
    ImageFile ret;
    ret.isNull = true;

    if (!isImage(fileInfo.filePath())) {
        // Skip non image files
        return ret;
    }

//    kDebug() << "FOUND IMAGE" << fileInfo.filePath();
    ret.isNull = false;
    ret.filePath = fileInfo.filePath();
    return ret;

//    // Get the unique id generated from the file path
//    QString encodedPath = uniqueId(fileInfo.filePath());

//    // Check if the document is on the DataBase
//    Xapian::Document document = findDocument(encodedPath);
//    if (document.get_docid() == 0) {
//        kDebug() << "No document with id" << encodedPath;
//        m_notImported << fileInfo.filePath();
//        // emit notImported()

//        // It's said that adding the docs sorted it faster.. (change in future)
////        m_rwXapianDB->replace_document(encodedPath.toStdString(),
////                                       createDocument(encodedPath, fileInfo));
//    } else {
////        kDebug() << "Document with id" << encodedPath << "has docid" << document.get_docid();

//        if (!document.get_value(Rating).empty()) {
//            kDebug() << "Has RATING" << QString::fromStdString(document.get_value(Rating));
//        }

//        if (fileInfo.lastModified().toMSecsSinceEpoch() !=
//                QString::fromStdString(document.get_value(LastModified)).toLongLong()) {
//            // File has changed, update it's entry
//            kDebug() << "StatIfo Changed"
//                     << QString::fromStdString(document.get_value(LastModified)).toLongLong()
//                     << fileInfo.lastModified().toMSecsSinceEpoch();
//            m_rwXapianDB->replace_document(encodedPath.toStdString(),
//                                           createDocument(encodedPath, fileInfo));

//            // Start the commit timer as this document has been updated
//            m_commitTimer->start();
//        }
    //    }
}

void ImageRequest::getEvents()
{
    kDebug();
    Xapian::ValueCountMatchSpy spy0(Database::Event);
    Xapian::Enquire enquire(*m_db.database());
    enquire.add_matchspy(&spy0);

    enquire.set_query(Xapian::Query::MatchAll);

    // Get matches one result is enough
    enquire.get_mset(0, 1, m_db.database()->get_doccount());
    for (Xapian::TermIterator i = spy0.values_begin(); i != spy0.values_end(); ++i) {
        kDebug() << QString::fromStdString(*i) << ": " << i.get_termfreq();
        Event eventObj;
        eventObj.name = QString::fromStdString(*i);
        eventObj.photos = i.get_termfreq();

        Xapian::Enquire enquireDoc(*m_db.database());
        Xapian::Query eventQuery(Xapian::Query::OP_VALUE_RANGE,
                                 Database::Event,
                                 *i,
                                 *i);
        Xapian::Query coverQuery(Xapian::Query::OP_VALUE_RANGE,
                                 Database::EventCover,
                                 "1",
                                 "1");
        Xapian::Query query(Xapian::Query::OP_AND,
                            eventQuery,
                            coverQuery);
        enquireDoc.set_query(query);
        Xapian::MSet matches = enquireDoc.get_mset(0, 1, m_db.database()->get_doccount());
        for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i) {
            Xapian::Document doc = i.get_document();
            ImageFile image;
            image.docId = doc;
            image.filePath = QString::fromStdString(doc.get_value(Database::FilePath));
            kDebug() << "FOUND Document ID " << QString::number(*i) << "\t" <<
                    QString::number(i.get_percent()) << "% [" <<
                        QString::fromStdString(doc.get_data()) << "]";
            eventObj.cover = image;
        }

        emit eventItem(eventObj);
    }
}

void ImageRequest::getImagesFromEvent(const QString &eventName)
{
    kDebug() << eventName;
    Xapian::Enquire enquire(*m_db.database());
    Xapian::Query query(Xapian::Query::OP_VALUE_RANGE,
                        Database::Event,
                        eventName.toStdString(),
                        eventName.toStdString());

    enquire.set_query(query);
    Xapian::MSet matches = enquire.get_mset(0,
                                            m_db.database()->get_doccount(),
                                            m_db.database()->get_doccount());
    for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i) {
        Xapian::Document doc = i.get_document();
        ImageFile image;
        image.docId = doc;
        image.filePath = QString::fromStdString(doc.get_value(Database::FilePath));
        kDebug() << "FOUND Document ID " << QString::number(*i) << "\t" <<
                QString::number(i.get_percent()) << "% [" <<
                    QString::fromStdString(doc.get_data()) << "]";
        emit file(image);
    }
}
